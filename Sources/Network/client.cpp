#include "OpenGL_Manager.hpp"
#include "callbacks.hpp"
#include "logs.hpp"

void OpenGL_Manager::sendPacket( t_packet_data data, size_t size )
{
	if (!_socket->send(_socket->getServerAddress(), &data, size)) {
		_socket = nullptr;
		_menu->setState(menu::error);
		_menu->setErrorStr({"Disconnected from server", "Timeout I guess"});
	}
}

void OpenGL_Manager::handlePackets( void )
{
	while (true) { // read incoming packets
		Address sender;
		t_packet_data buffer;

		ssize_t bytes_read = _socket->receive(sender, &buffer, sizeof(buffer));
		
		if (bytes_read <= 0) {
			break;
		}
		PACKETLOG(LOG("received " << bytes_read << " bytes"));

		// process packet
		switch (buffer.action & mask::network::action_type) {
			case packet_id::server::login:
				break ;
			case packet_id::server::ping:
				break ;
			default:
				MAINLOG(LOGERROR("Unrecognised packet action: " << buffer.action << ", sent with data: |" << buffer.data << "|"));
		}
	}

	if (!_player) {
		sendPacket({packet_id::client::login}, sizeof(unsigned short));
	}
}

void OpenGL_Manager::handleClientInputs( void )
{
	// quit program
	if (inputs::key_down(inputs::quit_program)) {
		glfwSetWindowShouldClose(_window, GL_TRUE);
		return ;
	}
}

void OpenGL_Manager::runClient( void )
{
	utils::shader::check_glstate("entering client loop\n", true);

	while (!glfwWindowShouldClose(_window) && _socket)
	{
		handleTime(true); // true to avoid player tickUpdate

		if (_time.tickUpdate) {
			_time.redTickUpdate = DayCycle::Get()->tickUpdate();

			handlePackets();
		}

		handleClientInputs();
		// handleDraw();
		handleUI();
		// if (_paused) {
		// 	handleMenu(_time.nbTicks == 1 && _time.tickUpdate);
		// } else {
			_ui->textToScreen(_menu->getState() >= menu::pause);
		// }
		handleChunkDeletion();
		_time.previousFrame = _time.currentTime;
		glfwSwapBuffers(_window);
		glfwPollEvents();
	}

	utils::shader::check_glstate("exiting client loop\n", true);
}
