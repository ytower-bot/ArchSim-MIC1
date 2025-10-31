
### Microarquitetura

- **Via de Dados**
	- Registradores
	- ULA
	- Latch A / B
		- **Função**
			- Armazenar o valor de um dos 16 registradores.
		- **Saída**
			- **Latch A:** enviar o valor armazenado para a entrada A da ULA, passando pelo AMUX.
			- **Latch B:** enviar o valor armazenado para o MAR e para a entrada B da ULA.
		- **Estrutura**
			- **``data``**: Valor armazenado.
		 - **Definição de estrutura e funções**
			 ```
			 typedef struct latch {
				 int data[16];
			 } latch;
			 ```
	- Decoder A / B
		- **Função**
			- Fazer a seleção de qual registrador será o valor armazenado em cada Latch.
		- **Saída**
			- Carregar o valor do registrador selecionado para o Latch correspondente.
		- **Estrutura**
			- **``registers``**: Vetor que armazena o ponteiro de cada registrador;
			- **``control``**: Sinal de controle para seleção do registrador.
		- **Sinais de controle**
			 - O sinal de controle control é um binário de 4 bits que corresponde, em decimal, à posição no vetor de registradores que será selecionado para carregamento do Latch.
		 - **Definição de estrutura e funções**
			 ```
			 typedef struct decoder {
				 register* registers[16];
				 int control[4];
			 } decoder;
			 ```
			 - `void run_decoder(decoder* d, latch* l)`: executa a operação do decodificador.
	- Decoder C
		- **Função**
			- Fazer a seleção de qual registrador será armazenado o valor do deslocador.
		- **Saída**
			- Carregar o valor do deslocador para o registrador selecionado.
		- **Estrutura**
			- **``registers``**: Vetor que armazena o ponteiro de cada registrador;
			- **``control_c``**: Sinal de controle C para seleção do registrador;
			- **`control_enc`**: Sinal de controle ENC que habilita a escrita nos registradores.
		- **Sinais de controle**
			 - O sinal de controle control_c é um binário de 4 bits que corresponde, em decimal, à posição no vetor de registradores que será selecionado para armazenar o valor do deslocador.
			 - O sinal de controle ENC permite, ou não, a escrita nos registradores.
				- **`0`**: Não permite a escrita;
				- **`1`**: Permite a escrita.
		 - **Definição de estrutura e funções**
			 ```
			 typedef struct decoderC {
				 register* registers[16];
				 int control_c[4];
				 int control_enc;
			 } decoderC;
			 ```
			 - `void run_decoderC(decoderC* d, shifter* s)`: executa a operação do decodificador C.
	- MAR
		- **Função**
			- Armazenar o endereço de memória ao qual será feito a operação de leitura ou escrita. 
		- **Saída**
			- Envia o endereço para acesso à memória.
		- **Estrutura**
			- **``control_mar``**: Sinal de controle MAR (0 ou 1);
			- **``address``**: Endereço de memória armazenado;
		- **Sinais de controle**
			 - O sinal de controle MAR define se o MAR será carregado do Latch B:
				 - **``0``**: Não é carregado;
				 - **``1``**: Recebe o endereço do Latch B.
		 - **Definição de estrutura e funções**
			 ```
			 typedef struct mar {
				 int control_mar;
				 int address[12];
			 } mar;
			 ```
			 - `void run_mar(mar* a, latch* lB)`: faz a operação do mar, carregando o valor do Latch B se determinado.
	- MBR
		- **Função**
			- Armazenar um valor que veio da memória principal ou será escrito na memória.
		- **Saída**
			- Enviar o valor armazenado ou para a memória principal ou para o AMUX.
		- **Estrutura**
			- **``control_rd``**: Sinal de controle RD para leitura da memória (0 ou 1);
			- **`control_wr`**: Sinal de controle WR para escrita na memória (0 ou 1);
			- **`control_mbr`**: Sinal de controle MBR para carregar o valor do deslocador no MBR (0 ou 1);
			- **``data``**: Valor armazenado.
		- **Sinais de controle**
			 - O sinal de controle RD determina se foi solicitado uma leitura da memória:
				 - **``0``**: Nenhuma leitura;
				 - **`1`**: Carrega o MBR a partir da memória.
			- O sinal de controle WR determina se foi solicitado uma escrita na memória:
				 - **``0``**: Nenhuma escrita;
				 - **`1`**: Escreve o conteúdo de MBR na memória.
			- O sinal de controle MBR se o MBR recebe o valor do deslocador:
				 - **``0``**: Não carrega;
				 - **`1`**: Carrega o MBR a partir do deslocador.
		 - **Definição de estrutura e funções**
			 ```
			 typedef struct mbr {
				 int control_rd;
				 int control_wr;
				 int control_mbr;
				 int data[16];
			 } mbr;
			 ```
			 - `void run_mbr(mar* a, mbr* b)`: executa a operação do MBR.
			 - `void m_read(mar* a, mbr* b)`: carrega o MBR a partir da memória.
			 - `void m_write(mar* a, mbr* b)`: escreve o valor do MBR na memória.
	- Deslocador
		- **Função**
			- Receber o resultado da ULA e fazer o deslocamento dos bits (à esquerda ou à direita) conforme comando da unidade de controle.
		- **Saída**
			- Envia o resultado ao barramento C e ao MBR.
		- **Estrutura**
			- **``control_sh``**: Sinal de controle SH (0 a 3);
			- **``data``**: Resultado da ULA.
		- **Sinais de controle**
			 - O sinal de controle SH define qual operação será executada pelo deslocador, obedecendo à convenção do MIC-1:
				 1. **``00``**: Nenhuma operação;
				 2. **``01``**: Desloca 1 bit à esquerda;
				 3. **``10``**: Desloca 1 bit à direita;
				 4. **``11``**:  Padrão de bits não usado.
		 - **Definição de estrutura e funções**
			 ```
			 typedef struct shifter {
				 int control_sh[2];
				 int data[16];
			 } shifter;
			 ```
			 - `void run_shifter(shifter* s, mbr* b, barrC* c)`: executa a operação do deslocador.
			 - `void lshift(shifter* s)`: executa o deslocamento à esquerda.
			 - `void rshift(shifter* s)`: executa o deslocamento à direita.
	- AMUX
		- **Função**
			- Fazer a passagem de valor ou do MBR ou do Latch A para a ULA, de acordo com o sinal da unidade de controle. 
		- **Saída**
			- Envia o valor escolhido para a entrada A da ULA.
		- **Estrutura**
			- **``control_amux``**: Sinal de controle AMUX (0 ou 1);
		- **Sinais de controle**
			 - O sinal de controle AMUX define qual será o valor passado para a ULA:
				 - **``0``**: Latch A;
				 - **``1``**: MBR.
		 - **Definição de estrutura e funções**
			 ```
			 typedef struct amux {
				 int control_amux;
			 } amux;
			 ```
			 - `void run_amux(amux* a, mbr* b, latch* lA, alu* u)`: faz a passagem do valor escolhido para a ULA.

- **Unidade de Controle**
	- MIR
		- **Função**
			- Armazenar e distribuir a microinstrução buscada da memória de controle através de sinais de controle. 
		- **Saída**
			- Envia os sinais de controle à todos os componentes do MIC-1.
		- **Estrutura**
			- **``data``**: Microinstrução armazenada;
		- **Sinais de controle**
			 - A MIR armazena uma microinstrução, que é dividida em sinais de controle da seguinte forma nas seguintes posições:
				 - **``[0]``**: Sinal AMUX;
				 - **`[1-2]`**: Sinal COND;
				 - **`[3-4]`**: Sinal ALU;
				 - **`[5-6]`**: Sinal SH;
				 - **`[7]`**: Sinal MBR;
				 - **`[8]`**: Sinal MAR;
				 - **`[9]`**: Sinal RD;
				 - **`[10]`**: Sinal WR;
				 - **`[11]`**: Sinal ENC;
				 - **`[12-15]`**: Sinal C;
				 - **`[16-19]`**: Sinal B;
				 - **`[20-23]`**: Sinal A;
				 - **`[24-31]`**: Sinal ADDR.
		 - **Definição de estrutura e funções**
			 ```
			 typedef struct mir {
				 int data[32];
			 } mir;
			 ```
			 - `void run_mir(mir* m, mbr* mb, mar* ma, mmux* mmu, amux* amu, shifter* s, alu* al, decoder* da, decoder* db, decoderC* dc)`: carrega os sinais de controle em todos os componentes.
	-  MPC
		- **Função**
			- Armazenar o endereço da próxima microinstrução a ser buscada na memória de controle.
		- **Saída**
			- Envia o endereço para acesso à memória de controle;
			- Também envia ao incrementador, que soma 1 ao endereço.
		- **Estrutura**
			- **``address``**: Endereço de memória armazenado.
		 - **Definição de estrutura e funções**
			 ```
			 typedef struct mpc {
				 int address[8];
			 } mpc;
			 ```
			 - `void run_mpc(mpc* p, mir* m)`: faz a leitura da próxima microinstrução e a armazena na MIR.
	- MMUX
		- **Função**
			- Selecionar o endereço da próxima microinstrução a ser executada e armazenar no MPC. Essa seleção é feita com base na decisão do controle de fluxo que pode ser sequencial ou condicional. 
		- **Saída**
			- Envia o endereço para o MPC.
		- **Estrutura**
			- **``control_cond``**: Sinal de controle COND (0 a 3);
			- **``alu_n``**: Sinal N da ULA (0 ou 1);
			- **``alu_z``**: Sinal Z da ULA (0 ou 1);
			
		- **Sinais de controle**
			 - O sinal de controle COND define qual será a condição para desvio do programa:
				 	 0. **``00``**: Nenhum desvio;
				 1. **``01``**: Desvia se N = 1;
				 2. **``10``**: Desvia se Z = 1;
				 3. **``11``**:  Desvia sempre.
		 - **Definição de estrutura e funções**
			 ```
			 typedef struct mmux {
				 int control_cond[2];
				 int alu_n;
				 int alu_z;
			 } mmux;
			 ```
			 - `void run_mmux(mmux* m, mpc* p)`: faz a operação do mmux.
### Memória
Implementar a lógica da memória principal
### Cache
?????
### Montador
traduzir o código de assembly pra binário

### Interface
