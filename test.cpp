#include "transfer_protocol.hpp"
#include "verification_algorithm.hpp"

#include <print>

#include <asio.hpp>

asio::awaitable<void> open_serial_port(asio::serial_port serial_port) {
  auto executor = co_await asio::this_coro::executor;
  (void)executor;

  if (serial_port.is_open()) {
    std::print("Serial port opened successfully.\n");
  } else {
     std::print("Failed to open serial port.\n");
     co_return;
  }

  gdut::packet_manager<gdut::crc16_algorithm> packet_manager;

  packet_manager.set_receive_function([](gdut::packet_manager<gdut::crc16_algorithm>::packet_t packet) {
    std::print("Received packet with payload size: {}\n", packet.size());
  });

  while (true) {
    std::array<std::uint8_t, 1024> buffer;
    std::size_t bytes_transferred = co_await asio::async_read(
      serial_port,
      asio::buffer(buffer),
      asio::use_awaitable);
    packet_manager.receive(buffer.begin(), buffer.begin() + bytes_transferred);
  }
}

int main() {
  try {
    asio::io_context io_context;
    asio::serial_port serial_port{io_context, "/dev/ttyS0"};
    serial_port.set_option(asio::serial_port_base::baud_rate(115200));
    serial_port.set_option(asio::serial_port_base::character_size(8));
    serial_port.set_option(asio::serial_port_base::parity(asio::serial_port_base::parity::none));
    serial_port.set_option(asio::serial_port_base::stop_bits(asio::serial_port_base::stop_bits::one));
    serial_port.set_option(asio::serial_port_base::flow_control(asio::serial_port_base::flow_control::none));
    asio::co_spawn(io_context, open_serial_port(std::move(serial_port)), asio::detached);
    io_context.run();
  } catch (const std::exception& e) {
    std::print("Error: {}\n", e.what());
  }
  return 0;
}