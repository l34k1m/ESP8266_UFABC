#ifndef _ESP8266_H
#define _ESP8266_H

/* a maior dificuldade de fazer a ESP funcionar direito sao as janelas de tempoSetup
  basta deixar bastante tempoSetup para receber da serial
  se nao ajustar muito bem, fica aparecendo uns "busy p..." ocasionalmente
  talvez melhore se aumentar o baud
  tais "busy p..." devem ser resultado de parada de recepção
  de dados seriais de comando anterior */

//	# MSG
//Wi-Fi
#define MSG_RESETAR "Resetar {"
#define MSG_AJUSTAR_MODO_OPERACAO "Ajustando modo de operacao {"
#define MSG_CONECTAR_PONTO_ACESSO "Conectando na rede {"
#define MSG_CRIAR_PONTO_ACESSO "Criando rede {"
#define MSG_MOSTRAR_IP "Conferindo IP {"
//TCP/IP
#define MSG_HABILITAR_MULTIPORTA "Iniciando modo multiporta {"
#define MSG_INICIAR_SERVIDOR "Iniciando porta 80 {"
#define MSG_ENVIAR_PAGINA "Enviando pagina {"
#define MSG_ENVIAR_CARGA "Enviando carga {"
#define MSG_FECHAR_CONEXAO "Fechando conexao {"
#define STRING_FIM_COMANDO "};\n"

//	# AT
//Wi-Fi
#define AT_RESETAR "AT+RST\r\n"
#define AT_MOSTRAR_IP "AT+CIFSR\r\n" // é a versão TCP/IP "AT+CIPSTA?\r\n" //
//TCP/IP
#define AT_HABILITAR_MULTIPORTA "AT+CIPMUX=1\r\n"
#define AT_INICIAR_SERVIDOR "AT+CIPSERVER=1,80\r\n"

//NOVOS
#define AT_FECHAR_CONEXAO "AT+CIPCLOSE\r\n"

//	# DELAY
//Wi-Fi
#define DELAY_RESETAR 650 //era 700 //era 750
#define DELAY_AJUSTAR_MODO_OPERACAO 100
#define DELAY_CONECTAR_PONTO_ACESSO 7000 //7000 modo 1, 1000 modo 3
#define DELAY_CRIAR_PONTO_ACESSO 950 //era 1000 //era 2000
#define DELAY_MOSTRAR_IP 1000
//TCP/IP
#define DELAY_HABILITAR_MULTIPORTA 25 //era 50
#define DELAY_INICIAR_SERVIDOR 50 //era 50
#define DELAY_ENVIAR_PAGINA 100 //original: 300
#define DELAY_ENVIAR_CARGA 250 //original: 600 (era ~205bytes) //a culpa desse valor ser tão discrepante é do tempo para fazer o print da string, ~400bytes/s
#define DELAY_ENVIAR_CARGA_GRANDE 750 //novo delay para garantir envio de algo colossal (por enquanto)
#define DELAY_FECHAR_CONEXAO 100 //original: 250
#define DELAY_FECHAR_CONEXAO_LONGO 300

//NOVOS
#define DELAY_CONECTAR_COMO_CLIENTE 500

#include <Arduino.h>
#include <SoftwareSerial.h>

class ESP8266 {
	public:
		long tempoSetup;
		unsigned short int contadorLoop;
		SoftwareSerial serial = SoftwareSerial(2, 3); //só para não funcionar sem o construtor ajustando ao criar objeto
		ESP8266(int, int);

		void resetar();
		void linhaFimComando();
		String enviarComando(const String&, unsigned short); //de 'String' para 'const String&' muda: passagem por referência(&) e uso de supervisor de modificação (const)
		void iniciarClienteBasico(const String&, const String&);
		void iniciarCliente(const String&, const String&);
		void conectarPontoAcesso(const String&, const String&);
		void iniciarPontoAcesso(const String&, const String&, int, int);
		void criarPontoAcesso(const String&, const String&, int, int);
		void ajustarModoOperacao(int);
		void mostrarIP();
		void habilitarMultiporta();
		void iniciarServidor();
		void conectarComoCliente(const String&); //recente //estava sem nada, apenas tinha o conteúdo interno plenamente funcional]
		void geraRequisicao(const String&, const String&); //recente
		void gerenciarRequisicoes(const String&); //passagem por referencia
		void enviarResumo(const String&); //recente
		void enviarPagina(const String&);
		void enviarCarga(const String&); //passagem por referencia
		void fecharConexao(); //recente
		void fecharConexao(const String&);
};

#endif