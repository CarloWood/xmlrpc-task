/**
 * xmlrpc-task -- AIStatefulTask submodule - XML RPC support.
 *
 * @file
 * @brief Implementation of XML_RPC_MethodCall.
 *
 * @Copyright (C) 2021  Carlo Wood.
 *
 * RSA-1024 0x624ACAD5 1997-01-26                    Sign & Encrypt
 * Fingerprint16 = 32 EC A7 B6 AC DB 65 A6  F6 F6 55 DD 1C DC FF 61
 *
 * This file is part of socket-task.
 *
 * Xmlrpc-task is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Xmlrpc-task is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with socket-task.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "sys.h"
#include "XML_RPC_MethodCall.h"
#include "evio/protocol/xmlrpc/Decoder.h"
#include "evio/protocol/xmlrpc/Encoder.h"
#include "evio/protocol/http.h"
#include "utils/debug_ostream_operators.h"

namespace task {

char const* XML_RPC_MethodCall::state_str_impl(state_type run_state) const
{
  switch(run_state)
  {
    AI_CASE_RETURN(XML_RPC_MethodCall_start);
    AI_CASE_RETURN(XML_RPC_MethodCall_check_status_code);
    AI_CASE_RETURN(XML_RPC_MethodCall_done);
  }
  ASSERT(false);
  return "UNKNOWN STATE";
}

void XML_RPC_MethodCall::multiplex_impl(state_type run_state)
{
  switch (run_state)
  {
    case XML_RPC_MethodCall_start:
    {
      // Initialize.
      auto socket = evio::create<evio::Socket>();
      socket->set_source(m_output_stream);
      socket->set_protocol_decoder(m_input_decoder);
      m_connect_to_end_point->set_socket(std::move(socket));
      m_connect_to_end_point->on_connected([this](bool success){
          if (success)
          {
            m_output_stream <<
                "POST / HTTP/1.1\r\n"
                "Host: misfitzgrid.com:8002\r\n"
                "Accept: */*\r\n"
                "Accept-Encoding:\r\n"
                "Connection: close\r\n"
                "Content-Type: text/xml\r\n";

            std::stringstream ss;
            evio::protocol::xmlrpc::Encoder encoder(ss);

            try
            {
              encoder << *m_request;
            }
            catch (AIAlert::Error const& error)
            {
              Dout(dc::warning, error);
            }

            std::string const& body = ss.str();
            m_output_stream <<
                "Content-Length: " << body.size() << "\r\n\r\n" <<
                body;

            m_connect_to_end_point->get_socket()->flush_output_device();
          }
        });
      m_connect_to_end_point->run(this, 1);
      // Wait until m_connect_to_end_point has finished successfully, then continue at state check_status_code.
      set_state(XML_RPC_MethodCall_check_status_code);
      wait(1);
      break;
    }
    case XML_RPC_MethodCall_check_status_code:
      if (m_input_decoder.get_status_code() != 200)
      {
        Dout(dc::warning, "Received HTTP status code is " << m_input_decoder.get_status_code());
        abort();
        break;
      }
      set_state(XML_RPC_MethodCall_done);
      [[fallthrough]];
    case XML_RPC_MethodCall_done:
      finish();
      break;
  }
}

} // namespace task
