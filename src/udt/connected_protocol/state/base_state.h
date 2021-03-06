#ifndef UDT_CONNECTED_PROTOCOL_STATE_BASE_STATE_H_
#define UDT_CONNECTED_PROTOCOL_STATE_BASE_STATE_H_

#include <memory>

#include <boost/chrono.hpp>

#include "udt/common/error/error.h"

#include "udt/connected_protocol/io/write_op.h"
#include "udt/connected_protocol/io/read_op.h"
#include "udt/connected_protocol/logger/log_entry.h"

namespace connected_protocol {
namespace state {

template <class Protocol>
class BaseState {
 public:
  using Ptr = std::shared_ptr<BaseState>;
  using ConnectionDatagram = typename Protocol::ConnectionDatagram;
  using ConnectionDatagramPtr = std::shared_ptr<ConnectionDatagram>;
  using ControlDatagram = typename Protocol::GenericControlDatagram;
  using SendDatagram = typename Protocol::SendDatagram;
  using DataDatagram = typename Protocol::DataDatagram;
  using Clock = typename Protocol::clock;
  using TimePoint = typename Protocol::time_point;
  using Timer = typename Protocol::timer;

 public:
  enum type { CLOSED, CONNECTING, ACCEPTING, CONNECTED, TIMEOUT };

 public:
  virtual type GetType() = 0;

  boost::asio::io_service& get_io_service() { return io_service_; }

  virtual void Init() {}

  virtual ~BaseState() {}

  virtual void Stop() {}

  virtual void Close() {}

  virtual void PushReadOp(
      io::basic_pending_stream_read_operation<Protocol>* read_op) {
    // Drop op
    auto do_complete = [read_op]() {
      read_op->complete(
          boost::system::error_code(::common::error::not_connected,
                                    ::common::error::get_error_category()),
          0);
    };
    io_service_.post(do_complete);
  }

  virtual void PushWriteOp(io::basic_pending_write_operation* write_op) {
    // Drop op
    auto do_complete = [write_op]() {
      write_op->complete(
          boost::system::error_code(::common::error::not_connected,
                                    ::common::error::get_error_category()),
          0);
    };
    io_service_.post(do_complete);
  }

  virtual bool HasPacketToSend() { return false; }

  virtual SendDatagram* NextScheduledPacket() { return nullptr; }

  virtual void OnConnectionDgr(ConnectionDatagramPtr p_connection_dgr) {
    // Drop dgr
  }

  virtual void OnControlDgr(ControlDatagram* p_control_dgr) {
    // Drop dgr
  }

  virtual void OnDataDgr(DataDatagram* p_datagram) {
    // Drop dgr
  }

  virtual void Log(connected_protocol::logger::LogEntry* p_log) {}

  virtual void ResetLog() {}

  virtual double PacketArrivalSpeed() { return 0.0; }

  virtual double EstimatedLinkCapacity() { return 0.0; }

  virtual boost::chrono::nanoseconds NextScheduledPacketTime() {
    return boost::chrono::nanoseconds(0);
  }

 protected:
  BaseState(boost::asio::io_service& io_service) : io_service_(io_service) {}

 private:
  boost::asio::io_service& io_service_;
};

}  // state
}  // connected_protocol

#endif  // UDT_CONNECTED_PROTOCOL_STATE_BASE_STATE_H_
