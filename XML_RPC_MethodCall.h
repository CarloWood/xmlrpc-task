/**
 * xmlrpc-task -- AIStatefulTask submodule - XML RPC support.
 *
 * @file
 * @brief Perform a XML RPC call. Declaration of class XML_RPC_MethodCall.
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

#pragma once

#include "statefultask/AIStatefulTask.h"
#include "socket-task/ConnectToEndPoint.h"
#include "evio/protocol/http.h"
#include "evio/protocol/XML_RPC_Decoder.h"
#include "evio/protocol/xmlrpc/ElementDecoder.h"
#include "evio/protocol/xmlrpc/Request.h"
#include "debug.h"

namespace task {

/**
 * The XML RPC task.
 *
 * Before calling @link group_run run()@endlink, call () to pass needed parameters.
 */
class XML_RPC_MethodCall : public AIStatefulTask
{
 protected:
  /// The base class of this task.
  using direct_base_type = AIStatefulTask;

  /// The different states of the stateful task.
  enum xml_rpc_method_call_state_type {
    XML_RPC_MethodCall_start = direct_base_type::state_end,
    XML_RPC_MethodCall_connect_begin,
    XML_RPC_MethodCall_connected,
    XML_RPC_MethodCall_connect_failed,
    XML_RPC_MethodCall_done
  };

  boost::intrusive_ptr<task::ConnectToEndPoint> m_connect_to_end_point;
  std::shared_ptr<xmlrpc::Request const> m_request;
  std::shared_ptr<xmlrpc::ElementDecoder> m_response;
  evio::protocol::http::ResponseHeadersDecoder m_input_decoder;
  evio::protocol::xmlrpc::ResponseDecoder m_xml_rpc_decoder;
  XML_RPC_Decoder m_xml_rpc_decoder;
  evio::OutputStream m_output_stream;

 public:
  /// One beyond the largest state of this task.
  static state_type constexpr state_end = XML_RPC_MethodCall_done + 1;

  /// Construct an XML_RPC_MethodCall object.
  XML_RPC_MethodCall(CWDEBUG_ONLY(bool debug = false)) : CWDEBUG_ONLY(AIStatefulTask(debug), )
                                                         m_connect_to_end_point(new task::ConnectToEndPoint),
                                                         m_input_decoder({{"text/xml", m_xml_rpc_decoder}})
    { DoutEntering(dc::statefultask(mSMDebug), "XML_RPC_MethodCall() [" << (void*)this << "]"); }

  /**
   * Set the end point to connect to.
   *
   * @param end_point The internet end point that should be connected to.
   */
  void set_end_point(AIEndPoint&& end_point) { m_connect_to_end_point->set_end_point(std::move(end_point)); }

  AIEndPoint const& get_end_point() const { return m_connect_to_end_point->get_end_point(); }

  void set_request_object(std::shared_ptr<xmlrpc::Request const> request) { m_request = std::move(request); }

  std::shared_ptr<xmlrpc::Request const> const& get_request_object() const { return m_request; }

  void set_response_object(std::shared_ptr<xmlrpc::ElementDecoder> response) { m_response = std::move(response); m_xml_rpc_decoder.init(m_response.get()); }

  std::shared_ptr<xmlrpc::ElementDecoder> const& get_response_object() const { return m_response; }

 protected:
  /// Call finish() (or abort()), not delete.
  ~XML_RPC_MethodCall() { DoutEntering(dc::statefultask(mSMDebug), "~XML_RPC_MethodCall() [" << (void*)this << "]"); }

  /// Implemenation of state_str for run states.
  char const* state_str_impl(state_type run_state) const override;

  /// Handle mRunState.
  void multiplex_impl(state_type run_state) override;
};

} // namespace task
