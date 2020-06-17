#include <Arduino.h> //o que tanto será que precisa dela? Só sei que precisa, infelizmente
#include <SoftwareSerial.h> //necessária para emular uma serial no Arduino

#include "ESP8266.h"

ESP8266::ESP8266(int RX, int TX) {
	SoftwareSerial serial = SoftwareSerial(RX, TX);
	this->serial = serial;
}

void ESP8266::resetar() {
	Serial.println(F(MSG_RESETAR));
	enviarComando(F(AT_RESETAR), DELAY_RESETAR); //reseta o módulo
	linhaFimComando();
}

void ESP8266::linhaFimComando() {
	Serial.println(F(STRING_FIM_COMANDO)); //o F funciona porque o valor a ser salvo na flash será aplicado em pré-processamento
}

String ESP8266::enviarComando(const String& comandoAT, unsigned short timeout) { //nenhuma operacao leva mais que 65s, por isso timeout -> unsigned short
	// Envio dos comandos AT para o modulo
	String resposta;
	resposta.reserve(72); //serve para alocar memória para a stringde dados recebidos da ESP
	//foi escolhido o tamanho de 72bytes porque cobre a maioria dos usos

	serial.print(comandoAT); //informa o comando AT à placa
	long limite = millis() + timeout;
	while (limite > millis()) {
		while (serial.available()) {
			// Quando a ESP tem algo a mostrar no buffer, o valor é recolhido
			char c = serial.read(); // read the next character.
			resposta += c;
		}
	}
	Serial.print(resposta); //Serial.print(resposta.length()); //poderia ser interessante
	return resposta;
}

void ESP8266::iniciarClienteBasico(const String& ssid, const String& pwd) {
	ajustarModoOperacao(1); //se é cliente(1), servidor(2) ou ambos(3); experimentei sérios problemas tentando usar o modo 3 //estava como 3 (?!
	conectarPontoAcesso(ssid, pwd);
}

void ESP8266::iniciarCliente(const String& ssid, const String& pwd) {
	ajustarModoOperacao(1); //se é cliente(1), servidor(2) ou ambos(3); experimentei sérios problemas tentando usar o modo 3 //estava como 3 (?!
	conectarPontoAcesso(ssid, pwd);
	habilitarMultiporta();
	iniciarServidor();
}

void ESP8266::conectarPontoAcesso(const String& ssid, const String& pwd) {
	//String comandoAT = "AT+CWJAP_CUR=\"" + ssid + "\",\"" + pwd + "\"\r\n";
	String comandoAT;
	comandoAT.reserve(50);
	comandoAT += F("AT+CWJAP_CUR=\"");
	comandoAT += ssid;
	comandoAT += F("\",\"");
	comandoAT += pwd;
	comandoAT += F("\"\r\n");
	Serial.println(F(MSG_CONECTAR_PONTO_ACESSO));
	enviarComando(comandoAT, DELAY_CONECTAR_PONTO_ACESSO);
	linhaFimComando();
}

void ESP8266::iniciarPontoAcesso(const String& ssid, const String& pwd, int canal, int nivelSeguranca) {
	ajustarModoOperacao(2);
	criarPontoAcesso(ssid,pwd,canal,nivelSeguranca);
	habilitarMultiporta();
	iniciarServidor();
}

void ESP8266::criarPontoAcesso(const String& ssid, const String& pwd, int canal, int nivelSeguranca) {
	//nivelSeguranca = OPEN(0), WEP(1), WPA_PSK(2), WPA2_PSK(3), WPA_WPA2_PSK(4)
	//String comandoAT = "AT+CWSAP_CUR=\"" + ssid + "\",\"" + pwd + "\"," + String(canal) + "," + String(nivelSeguranca) + "\r\n";
	String comandoAT;
	comandoAT.reserve(64); //nos testes tinha um tamanho aproximado de 46
	comandoAT += F("AT+CWSAP_CUR=\"");
	comandoAT += ssid;
	comandoAT += F("\",\"");
	comandoAT += pwd;
	comandoAT += F("\",");
	comandoAT += String(canal);
	comandoAT += F(",");
	comandoAT += String(nivelSeguranca);
	comandoAT += F("\r\n");
	Serial.println(F(MSG_CRIAR_PONTO_ACESSO));
	enviarComando(comandoAT, DELAY_CRIAR_PONTO_ACESSO);
	linhaFimComando();
}

void ESP8266::ajustarModoOperacao(int modoOperacao) { //1 é cliente, 2 é servidor, 3 é ambos
	Serial.println(F(MSG_AJUSTAR_MODO_OPERACAO));
	//String comandoAT = "AT+CWMODE=" + String(modoOperacao) + "\r\n";
	String comandoAT;
	comandoAT.reserve(16); //dá uns 15, em média
	comandoAT += F("AT+CWMODE=");
	comandoAT += String(modoOperacao);
	comandoAT += F("\r\n");
	enviarComando(comandoAT, DELAY_AJUSTAR_MODO_OPERACAO);
	linhaFimComando();
}

void ESP8266::mostrarIP() {
	Serial.println(F(MSG_MOSTRAR_IP));
	enviarComando(F(AT_MOSTRAR_IP), DELAY_MOSTRAR_IP);
	linhaFimComando();
}

void ESP8266::habilitarMultiporta() { // Configura para multiplas conexoes
	Serial.println(F(MSG_HABILITAR_MULTIPORTA));
	enviarComando(F(AT_HABILITAR_MULTIPORTA), DELAY_HABILITAR_MULTIPORTA);
	linhaFimComando();
}

void ESP8266::iniciarServidor() { // Inicia o web server na porta 80
	Serial.println(F(MSG_INICIAR_SERVIDOR));
	enviarComando(F(AT_INICIAR_SERVIDOR), DELAY_INICIAR_SERVIDOR);
	linhaFimComando();
}

void ESP8266::conectarComoCliente(const String& ip) {
	String comandoAT;
	comandoAT.reserve(44);
	comandoAT += F("AT+CIPSTART=\"TCP\",\"");
	comandoAT += ip;
	comandoAT += F("\",80\r\n");
	Serial.println(F("Conectando-se como cliente {"));
	enviarComando(comandoAT, DELAY_CONECTAR_COMO_CLIENTE);
	linhaFimComando();
}

void ESP8266::geraRequisicao(const String& url, const String& dadosSensores) { //uma das coisas mais medonhas ocorreu quando foi esquecido o ESP8266::
  String requisicaoPost;
  requisicaoPost.reserve(200);
  requisicaoPost += F("POST ");
  requisicaoPost += url;
  requisicaoPost += F(" HTTP/1.1\r\n");
  requisicaoPost += "Host: 172.31.121.41\r\n"; //foi substituída por uma linha que tem um nome no lugar de IP, talvez seja apenas um apêndice
  //requisicaoPost += F("Host: ESP\r\n");
  requisicaoPost += "Accept: */*\r\n"; //aparentemente não é linha obrigatória
  //requisicaoPost += "\r\n\r\n"; //aparentemente não é linha obrigatória
  requisicaoPost += F("Content-Type: application/x-www-form-urlencoded\r\n");
  requisicaoPost += F("Content-Length: ");
  requisicaoPost += dadosSensores.length();
  requisicaoPost += F("\r\n");
  requisicaoPost += F("\r\n");
  requisicaoPost += dadosSensores;
  requisicaoPost += F("\r\n");
  //Serial.print(requisicaoPost);
  enviarResumo(requisicaoPost);
}

void ESP8266::gerenciarRequisicoes(const String& paginaWeb) {
	if (serial.available()) { // Verifica se o ESP8266 tem dados a serem avaliados
		if (serial.find("+IPD,")) { //verifica se recebeu um comando específico de requisição de página
			delay(250); //dá problema se tirar, só sei isso... //original:250
			contadorLoop++;
			int connectionId = serial.read() - 48; //numero-sugestao de conexao recebido

			/*
			String paginaWeb;
			paginaWeb += F("<!DOCTYPE html>\n<html>\n<head>\n</head>\n<body>\n");
			paginaWeb += F("\n</body>");
			*/

			String stringInformarEnvio;
			stringInformarEnvio.reserve(16); //dava 16 efetivamente (considerando que paginaWeb.length = 200 posicoes (3 dígitos))
			stringInformarEnvio += F("AT+CIPSEND="); //usando sendbuffer talvez seja mais rápido
			stringInformarEnvio += String(connectionId);
			stringInformarEnvio += F(",");
			stringInformarEnvio += String(paginaWeb.length());
			stringInformarEnvio += F("\r\n");

			enviarPagina(stringInformarEnvio); //integrar
			enviarCarga(paginaWeb); //integrar
			String comandoATFecharConexao = F("AT+CIPCLOSE=");
			comandoATFecharConexao += String(connectionId) + "\r\n";
			fecharConexao(comandoATFecharConexao);

			//delay(1000); //margem de segurança para garantir fluxo de saida da placa???
		}
	}
}

void ESP8266::enviarResumo(const String& paginaWeb) {
	String stringInformarEnvio;
	stringInformarEnvio.reserve(16); //dava 16 efetivamente (considerando que paginaWeb.length = 200 posicoes (3 dígitos))
	stringInformarEnvio += F("AT+CIPSEND="); //usando sendbuffer talvez seja mais rápido
	stringInformarEnvio += String(paginaWeb.length());
	stringInformarEnvio += F("\r\n");
	enviarPagina(stringInformarEnvio); //integrar
	enviarCarga(paginaWeb); //integrar
}

void ESP8266::enviarPagina(const String& stringInformarEnvio) {
	Serial.println(F(MSG_ENVIAR_PAGINA));
	enviarComando(stringInformarEnvio, DELAY_ENVIAR_PAGINA);
	linhaFimComando();
}

void ESP8266::enviarCarga(const String& paginaWeb) {
	Serial.println(F(MSG_ENVIAR_CARGA));
	enviarComando(paginaWeb, DELAY_ENVIAR_CARGA_GRANDE); //original: DELAY_ENVIAR_CARGA
	linhaFimComando();
}

void ESP8266::fecharConexao() {
	Serial.println(F(MSG_FECHAR_CONEXAO));
	enviarComando(AT_FECHAR_CONEXAO, DELAY_FECHAR_CONEXAO_LONGO); //o 5 serve para fechar toda e qualquer conexão
	linhaFimComando();
}

void ESP8266::fecharConexao(const String& comandoATFecharConexao) {
	Serial.println(F(MSG_FECHAR_CONEXAO));
	enviarComando(comandoATFecharConexao, DELAY_FECHAR_CONEXAO);
	linhaFimComando();
}