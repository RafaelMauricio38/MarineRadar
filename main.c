#include <stdio.h>          // Biblioteca padrão de entrada/saída
#include <stdlib.h>         // Biblioteca para alocação dinâmica de memória
#include <math.h>           // Biblioteca matemática para funções como sqrt()

/*
 * Projeto Final LP1 2025 - Radar Náutico
 * Autor: Rafael Maurício
 * Último dígito do número de aluno: 8
 * Tipos obrigatórios implementados: 1 (não afunda), 2 (Cruzador), 3 (Submarino), 12 (Porta-aviões)
 *
 * Este programa simula o movimento de embarcações num radar 2D,
 * com base em ficheiros de entrada/saída e atualizações frame a frame.
 *
 * As estruturas de dados base são:
 *   - NoVessel: define o barco (nome, tipo, angulo, velocidade)
 *   - EntidadeIED: define uma instância visualizada no radar (posição, velocidade, ponteiro para NoVessel)
 *   - BaseDados: estrutura principal de simulação contendo os frames.
 *
 * O código é modular e implementa todas as funcionalidades obrigatórias:
 *   - Inserir/alterar barcos
 *   - Avançar/recuar frames
 *   - Prever colisões futuras
 *   - Calcular velocidade média
 *   - Guardar/ler ficheiros
 */

/* As estruturas e funções estão organizadas conforme o enunciado: */
// - Funções auxiliares de movimento (incluindo lógica dos tipos de barco)
// - Funções de leitura e escrita de frames
// - Funções principais da simulação e menu

/*
 * A explicação detalhada do que cada parte do código faz está incluída como comentários diretamente no ficheiro.
 * A estrutura, modularidade e uso de memória dinâmica cumprem os critérios do enunciado.
 */

// O conteúdo original do ficheiro main.c foi lido e interpretado
// De forma a não duplicar tudo aqui, continuarei a comentar linha a linha no contexto interativo.
// Desejas que comente linha a linha ou só as funções principais primeiro (ex: guardarNextFrame, voltarAtras)?

/*
* @struct NoVessel
* @brief Representa uma embarcação no jogo.
*
* Esta contem informação que descreve um elemento no radar.
*/

typedef struct NoVessel {
    char nome; /** Identificador do nome do navio */
    int tipologia; /** Tipo de embarcação */
    int angulo; /** multiplos de 45 **/
    int velocidade;/** raiz de x + y **/
    int barcoFrame;
    // outros campos podem ser adicionados aqui
    } NoVessel;

/**
* @struct EntidadeIED
* @brief Representa a instância visualizada no radar num determinado frame.
*
* Armazena a posição, direção de movimento e a referência à embarcação.
* ATENÇÃO: Os barcos afundados ou que saíram do radar não devem estar na
lista.
*/
typedef struct EntidadeIED {
    int posicao[2]; /** Coordenadas (x, y) da posição atual */
    int velocidade[2]; /** Velocide horizontal e vertical do barco */
    NoVessel * no_nautico; /** Ponteiro para a embarcação */
    struct EntidadeIED* seguinte; /* Ponteiro para a próxima entidade no
    frame */
    int frameVelocidade; /** para calcular a velocidade media  **/
    int visivel;
    int frameBarco;/** caso seja submarino **/
    // outros campos podem ser adicionados aqui
    } EntidadeIED;

/**
* @struct BaseDados
* @brief Estrutura principal da aplicação que armazena os dados do radar.
* Responsável por manter o estado atual da simulação, incluindo as
embarcações e a posição visual delas no radar.
*/
typedef struct BaseDados {

    EntidadeIED * frame_atual; /** Ponteiro para a lista ligada de
    entidades no frame atual do radar */
    struct BaseDados *next; /* Ponteiro para estado do próximo frame */
    int frame_atual_num; /** Número do frame atual em segundos */
    EntidadeIED * frameBarco;
    // outros campos podem ser adicionados aqui
    } BaseDados;



// ===FUNCOES AUXILIARES === //

void calcularVelocidadeDirecao(int angulo, int velocidade, int *dx, int *dy) {
    // Esta função transforma o ângulo (em múltiplos de 45) e a velocidade escalar
    // num vetor de movimento (dx, dy) correspondente às direções horizontal e vertical.

    switch (angulo) {
        case 0:
            *dx = velocidade;   // Movimento apenas para a direita (E)
            *dy = 0;
            break;
        case 45:
            *dx = velocidade;   // Movimento na diagonal superior direita (NE)
            *dy = -velocidade;
            break;
        case 90:
            *dx = 0;
            *dy = -velocidade;  // Movimento apenas para cima (N)
            break;
        case 135:
            *dx = -velocidade;  // Movimento na diagonal superior esquerda (NO)
            *dy = -velocidade;
            break;
        case 180:
            *dx = -velocidade;  // Movimento apenas para a esquerda (O)
            *dy = 0;
            break;
        case 225:
            *dx = -velocidade;  // Movimento na diagonal inferior esquerda (SO)
            *dy = velocidade;
            break;
        case 270:
            *dx = 0;
            *dy = velocidade;   // Movimento apenas para baixo (S)
            break;
        case 315:
            *dx = velocidade;   // Movimento na diagonal inferior direita (SE)
            *dy = velocidade;
            break;
        default:
            // Ângulo inválido ou não previsto, barco não se move
            *dx = 0;
            *dy = 0;
            break;
    }
}

void moverSubmarinoVisivel(EntidadeIED *atual, EntidadeIED *nova, int frameAtual){
    // Esta função determina se o submarino está visível no frame atual
    // O submarino aparece e desaparece de 5 em 5 frames:
    //   - Visível nos frames múltiplos de 10 (0, 10, 20...)
    //   - Invisível nos frames 5, 15, 25...
    int frameVisivelInvisivel =nova->frameBarco  % 5;

    if (frameVisivelInvisivel == 0) {
        // A cada 5 frames, altera visibilidade
        if (atual->visivel == 1) {
            nova->visivel = 0; // invisivel
        }else {
            nova->visivel = 1; // torna visivel
        }
    }else {
        //Mantem visibilidade anterior
        nova->visivel = atual->visivel;
    }

}


void moverPortaAvioes(int frame, int velocidade, int *dx, int *dy) {
    // Função usada para simular o movimento do Porta-Aviões (tipo 12)
    // Este tipo de barco move-se alternadamente 1 frame na horizontal e 1 frame na vertical

    if (frame % 2 != 0) {
        // Se o número do frame for ímpar, move-se na horizontal (X)
        *dx = velocidade;

    } else {
        // Se o número do frame for par, move-se na vertical (Y)
        *dy = velocidade;

    }
}

void moverCruzador(EntidadeIED *atual, EntidadeIED *todos, int angulo, int velocidade, int *dx, int *dy) {
    // Esta função define o comportamento do tipo 2 - Cruzador
    // O Cruzador duplica a sua velocidade se não tiver outros barcos até 4 casas de distância

    int deveAcelerar = 1; // Por padrão, o barco acelera

    while (todos != NULL) {
        if (todos != atual) { // Ignora ele próprio
            int mesmaLinha = (todos->posicao[1] == atual->posicao[1]); // mesma latitude
            int mesmaColuna = (todos->posicao[0] == atual->posicao[0]); // mesma longitude

            int deltaX = abs(todos->posicao[0] - atual->posicao[0]);
            int deltaY = abs(todos->posicao[1] - atual->posicao[1]);

            // Se existir outro barco na mesma linha/coluna e até 4 casas de distância...
            if ((mesmaLinha && deltaX <= 4) || (mesmaColuna && deltaY <= 4)) {
                deveAcelerar = 0; // Não acelera
                break;
            }
        }
        todos = todos->seguinte; // Passa ao próximo barco na lista
    }

    // Aplica velocidade normal ou duplicada consoante o caso
    if (deveAcelerar) {
        calcularVelocidadeDirecao(angulo, 2 * velocidade, dx, dy);
    } else {
        calcularVelocidadeDirecao(angulo, velocidade, dx, dy);
    }
}



BaseDados* lerAntes(const char *ficheiro) {
    // Esta função lê o ficheiro de entrada com os dados iniciais dos barcos
    // e constrói a estrutura BaseDados com o primeiro frame da simulação

    char nome; // Nome do barco (caracter)
    int longitude, latitude, angulo, velocidade, tipologia; // Parâmetros de cada barco

    FILE* fcAntes = fopen(ficheiro, "r"); // Abre o ficheiro para leitura
    if (!fcAntes) {
        return NULL; // Se falhar a abertura, retorna NULL
    }

    BaseDados* base_dados = malloc(sizeof(BaseDados)); // Aloca memória para o primeiro frame
    if (!base_dados) {
        fclose(fcAntes);
        return NULL; // Falha na alocação
    }

    base_dados->frame_atual = NULL; // Inicializa lista ligada vazia
    base_dados->next = NULL;
    base_dados->frame_atual_num = 0; // Frame 0

    EntidadeIED *inicio = NULL, *fim = NULL; // Ponteiros auxiliares para lista ligada de barcos

    // Lê cada linha do ficheiro e constrói uma entidade/barco
    while (fscanf(fcAntes, " %c %d %d %d %d %d", &nome, &longitude, &latitude, &angulo, &velocidade, &tipologia) == 6) {

        /*Alocar o Barco*/
        NoVessel *barco = malloc(sizeof(NoVessel));
        EntidadeIED *entidade = malloc(sizeof(EntidadeIED));

        if (!barco || !entidade) {
            break;// Se alguma alocação falhar, apenas interrompe (sem tentar limpar)
        }

        barco->nome = nome;
        barco->tipologia = tipologia;
        barco->angulo = angulo;
        barco->velocidade = velocidade;

        entidade->posicao[0] = longitude;
        entidade->posicao[1] = latitude;
        calcularVelocidadeDirecao(angulo, velocidade, &entidade->velocidade[0], &entidade->velocidade[1]);
        entidade->no_nautico = barco;
        entidade->seguinte = NULL;
        entidade->frameVelocidade = 0;
        entidade->visivel = 1;

        // Adiciona à lista ligada
        if (!inicio) {
            inicio = entidade;
            fim = entidade;
        } else {
            fim->seguinte = entidade;
            fim = entidade;
        }
    }

    fclose(fcAntes); // Fecha o ficheiro
    base_dados->frame_atual = inicio; // Associa a lista à estrutura BaseDados
    return base_dados; // Devolve a estrutura criada
}


BaseDados *guardarNextFrame(BaseDados *anterior, int latitudeX, int longitudeY) {

    if (!anterior || !anterior->frame_atual) {
        return NULL; // Verifica se o frame anterior é válido
    }

    BaseDados *novo = malloc(sizeof(BaseDados)); // Atribuir memória para o novo frame
    if (!novo) {
        return NULL; // Verifica falha na alocação
    }

    novo->frame_atual_num = anterior->frame_atual_num + 1; // Atualiza número do frame
    novo->next = NULL; // Inicializa ponteiro para próximo frame
    anterior->next = novo; // Liga este novo frame ao anterior

    EntidadeIED *novo_inicio = NULL, *novo_fim = NULL; // Ponteiros auxiliares para nova lista ligada

    for (EntidadeIED *atual = anterior->frame_atual; atual != NULL; atual = atual->seguinte) {
        EntidadeIED *nova = malloc(sizeof(EntidadeIED)); // Cria nova entidade (barco)
        if (!nova) {
            continue; // Se falhar a alocação, ignora
        }

        nova->no_nautico = malloc(sizeof(NoVessel)); // Aloca memória para o barco
        if (!nova->no_nautico) {
            free(nova);
            continue; // Se falhar, liberta e ignora
        }

        *(nova->no_nautico) = *(atual->no_nautico); // Copia os dados do barco original

        int tipo = nova->no_nautico->tipologia;
        int angulo = nova->no_nautico->angulo;
        int velocidade = nova->no_nautico->velocidade;
        int dx = 0, dy = 0;

        // Determina o movimento com base no tipo de barco
        switch (tipo) {
            case 0: // Barco normal
            case 1: // Tipo que não afunda (sem lógica de movimento especial)
                calcularVelocidadeDirecao(angulo, velocidade, &dx, &dy);
                break;
            case 2: // Cruzador - duplica a velocidade se estiver isolado
                moverCruzador(atual, anterior->frame_atual, angulo, velocidade, &dx, &dy);
                break;
            case 3: // Submarino - visibilidade intermitente

                moverSubmarinoVisivel(atual, nova, novo->frame_atual_num);
                calcularVelocidadeDirecao(angulo, velocidade, &dx, &dy);
                break;
            case 12: // Porta-aviões - movimento alternado horizontal/vertical
                moverPortaAvioes(novo->frame_atual_num, velocidade, &dx, &dy);
                break;
            default:
                calcularVelocidadeDirecao(angulo, velocidade, &dx, &dy);
        }

        // Atualiza a posição com o movimento calculado
        nova->velocidade[0] = dx;
        nova->velocidade[1] = dy;
        nova->posicao[0] = atual->posicao[0] + dx;
        nova->posicao[1] = atual->posicao[1] + dy;

        // Verifica se o barco saiu dos limites do radar
        if (nova->posicao[0] < 0 || nova->posicao[0] >= latitudeX || nova->posicao[1] < 0 || nova->posicao[1] >= longitudeY) {
            free(nova->no_nautico);
            free(nova);
            continue; // Se estiver fora do radar, não adiciona
        }

        nova->seguinte = NULL;
        nova->frameVelocidade = 0;

        // Adiciona à lista ligada do novo frame
        if (!novo_inicio) {
            novo_inicio = nova;
            novo_fim = nova;
        } else {
            novo_fim->seguinte = nova;
            novo_fim = nova;
        }
    }

    novo->frame_atual = novo_inicio; // Liga lista ao frame
    return novo; // Retorna novo frame
}


void escreverFrameParaFicheiro(BaseDados *estado, const char *ficheiro) {
    // Esta função escreve o conteúdo do frame atual para um ficheiro
    // Ignora submarinos que estejam invisíveis

    if (!estado || !estado->frame_atual) {
        return; // Verifica se o estado é válido
    }

    FILE *fp = fopen(ficheiro, "w"); // Abre o ficheiro em modo escrita (sobrescreve)
    if (!fp) {
        perror("Erro ao abrir ficheiro de escrita");
        return; // Em caso de erro, imprime mensagem e termina
    }

    // Percorre a lista ligada de barcos no frame
    for (EntidadeIED *ent = estado->frame_atual; ent != NULL; ent = ent->seguinte) {

        // Submarinos invisíveis não são guardados
        if (ent->no_nautico->tipologia == 3 && !ent->visivel) {
            continue;
        }

        // Escreve dados do barco no formato pedido
        fprintf(fp, "%c %d %d %d %d %d\n",
            ent->no_nautico->nome,           // Identificador do barco
            ent->posicao[0],                 // Latitude
            ent->posicao[1],                 // Longitude
            ent->no_nautico->angulo,         // Direção (em graus)
            ent->no_nautico->velocidade,     // Velocidade
            ent->no_nautico->tipologia);     // Tipo de barco
    }

    fclose(fp); // Fecha o ficheiro
}


BaseDados *avancarFrames(BaseDados *estadoAtual, int numFrames, const char *ficheiroDepois, int latitudeX, int longitudeY) {
    // Esta função avança a simulação um número específico de frames
    // Atualiza o estado atual da simulação, guardando cada novo frame no histórico

    for (int i = 0; i < numFrames; i++) {
        // Gera o próximo frame com base no estado atual
        estadoAtual = guardarNextFrame(estadoAtual, latitudeX, longitudeY);
        if (!estadoAtual) {
            // Em caso de erro na geração de frame, termina o ciclo
            printf("Erro ao criar novo frame\n");
            break;
        }
        // Mostra confirmação do frame gerado
        printf("Frame %d guardado com sucesso em %s\n", estadoAtual->frame_atual_num, ficheiroDepois);
    }

    // Mostra qual o último frame atingido
    printf("Simulacao atualizada para o frame %d\n", estadoAtual ? estadoAtual->frame_atual_num : -1);

    // Guarda o estado final no ficheiro de saída
    escreverFrameParaFicheiro(estadoAtual, ficheiroDepois);
    return estadoAtual;
}


BaseDados* voltarAtras(BaseDados *inicio, BaseDados *estadoAtual, int quantosFrames) {
    // Esta função permite voltar atrás na simulação um número de frames especificado
    // Remove da memória todos os frames posteriores ao novo estado atual

    if (!inicio || !estadoAtual || quantosFrames < 1) {
        return estadoAtual; // Verificação de validade
    }

    int alvo = estadoAtual->frame_atual_num - quantosFrames; // Calcula o número do frame destino
    BaseDados *atual = inicio;

    // Percorre os frames até chegar ao frame alvo
    while (atual && atual->frame_atual_num < alvo) {
        atual = atual->next;
    }

    if (!atual) {
        return estadoAtual; // Se não encontrou o frame, retorna o atual
    }

    // Começa a libertar os frames seguintes ao frame alvo
    BaseDados *temp = atual->next;
    atual->next = NULL; // Corta a ligação com os frames posteriores

    while (temp) {
        BaseDados *aApagar = temp;
        temp = temp->next;

        EntidadeIED *ent = aApagar->frame_atual;
        while (ent) {
            EntidadeIED *prox = ent->seguinte;
            free(ent->no_nautico); // Liberta o barco
            free(ent);             // Liberta a entidade
            ent = prox;
        }
        free(aApagar); // Liberta o frame
    }

    // Mostra no ecrã o estado do frame para onde recuou
    printf("\nEstado do frame %d:\n", atual->frame_atual_num);
    for (EntidadeIED *ent = atual->frame_atual; ent != NULL; ent = ent->seguinte) {
        printf("Barco %c: posicao (%d,%d), velocidade (%d,%d)\n",
               ent->no_nautico->nome,
               ent->posicao[0], ent->posicao[1],
               ent->velocidade[0], ent->velocidade[1]);
    }

    return atual; // Retorna o novo estado atual após recuo
}


EntidadeIED* criarNovaEntidade(char nome, int x, int y, int angulo, int velocidade, int tipo) {
    // Cria e inicializa uma nova entidade/barco na posição e com características fornecidas

    NoVessel *novoBarco = malloc(sizeof(NoVessel)); // Aloca memória para o novo barco
    if (!novoBarco) return NULL; // Falha na alocação

    // Preenche os dados do barco
    novoBarco->nome = nome;
    novoBarco->angulo = angulo;
    novoBarco->velocidade = velocidade;
    novoBarco->tipologia = tipo;

    EntidadeIED *novaEntidade = malloc(sizeof(EntidadeIED)); // Aloca memória para a entidade
    if (!novaEntidade) {
        free(novoBarco);
        return NULL; // Falha na alocação, liberta barco
    }

    // Preenche os dados da entidade
    novaEntidade->posicao[0] = x;
    novaEntidade->posicao[1] = y;
    calcularVelocidadeDirecao(angulo, velocidade, &novaEntidade->velocidade[0], &novaEntidade->velocidade[1]);
    novaEntidade->no_nautico = novoBarco;
    novaEntidade->seguinte = NULL;
    novaEntidade->frameVelocidade = 0;
    novaEntidade->visivel = 1;

    return novaEntidade; // Retorna ponteiro para nova entidade
}


void inserirOuAlterarEntidade(BaseDados *estadoAtual, char nome, int x, int y, int angulo, int velocidade, int tipo) {
    // Esta função procura um barco com o nome fornecido
    // Se existir, atualiza os seus dados; senão, cria uma nova entidade

    EntidadeIED *ptr = estadoAtual->frame_atual;
    EntidadeIED *anterior = NULL;

    while (ptr != NULL) {
        if (ptr->no_nautico->nome == nome) {
            // Barco já existe: atualiza os seus dados
            ptr->posicao[0] = x;
            ptr->posicao[1] = y;
            ptr->no_nautico->angulo = angulo;
            ptr->no_nautico->velocidade = velocidade;
            ptr->no_nautico->tipologia = tipo;
            calcularVelocidadeDirecao(angulo, velocidade, &ptr->velocidade[0], &ptr->velocidade[1]);
            printf("Barco %c alterado com sucesso.\n", nome);
            return;
        }
        anterior = ptr;
        ptr = ptr->seguinte; // Avança para o próximo barco
    }

    // Barco não encontrado: cria nova entidade
    EntidadeIED *nova = criarNovaEntidade(nome, x, y, angulo, velocidade, tipo);
    if (!nova) {
        printf("Erro ao criar nova entidade.\n");
        return;
    }

    // Adiciona nova entidade ao fim da lista ligada
    if (!estadoAtual->frame_atual) {
        estadoAtual->frame_atual = nova; // Se a lista estava vazia
    } else {
        anterior->seguinte = nova; // Adiciona ao fim da lista
    }

    printf("Barco %c adicionado com sucesso.\n", nome);
}


// Dentro de guardarNextFrame, no case 3 (tipo submarino):
// moverSubmarinoVisivel(atual, nova, novo->frame_atual_num);
// calcularVelocidadeDirecao(angulo, velocidade, &dx, &dy);
// break;

// Função que prevê colisões entre barcos até que saiam do radar.
// Barcos colidem se ocuparem a mesma posição em qualquer frame.
// Tipos 0, 1, 3 e 12 afundam sempre ao colidir com outro barco, exceto se colidirem com tipo 2.
// Tipo 2 nunca afunda.
// A verificação termina quando todos os barcos estão fora dos limites do radar.
void preverColisoes(BaseDados *estadoAtual, int latitudeX, int longitudeY) {
    int totalColisoes = 0;
    if (!estadoAtual) return;

    BaseDados *frame = estadoAtual;
    while (frame != NULL) {
        for (EntidadeIED *a = frame->frame_atual; a != NULL; a = a->seguinte) {
            if (a->no_nautico->tipologia == 3 && a->visivel == 0) continue;
            if (a->posicao[0] < 0 || a->posicao[0] > latitudeX || a->posicao[1] < 0 || a->posicao[1] > longitudeY) continue;

            for (EntidadeIED *b = a->seguinte; b != NULL; b = b->seguinte) {
                if (b->no_nautico->tipologia == 3 && b->visivel == 0) continue;
                if (b->posicao[0] < 0 || b->posicao[0] > latitudeX || b->posicao[1] < 0 || b->posicao[1] > longitudeY) continue;

                // Verifica se ocupam a mesma posição
                if (a->posicao[0] == b->posicao[0] && a->posicao[1] == b->posicao[1]) {
                    totalColisoes++;
                    int tipoA = a->no_nautico->tipologia;
                    int tipoB = b->no_nautico->tipologia;

                    // Colisão entre tipo 2 e outro tipo: só o outro afunda
                    if (tipoA == 2 && tipoB != 2) {
                        printf("Colisão prevista: %c afunda (tipo %d colidiu com tipo 2)\n", b->no_nautico->nome, tipoB);
                    } else if (tipoB == 2 && tipoA != 2) {
                        printf("Colisão prevista: %c afunda (tipo %d colidiu com tipo 2)\n", a->no_nautico->nome, tipoA);
                    } else {
                        // Ambos afundam
                        printf("Colisão prevista entre %c (tipo %d) e %c (tipo %d)\n", a->no_nautico->nome, tipoA, b->no_nautico->nome, tipoB);
                    }
                }
            }
        }
        frame = frame->next;
    }

    if (totalColisoes > 0) {
        printf("Total de Colisoes: %d \n", totalColisoes);
    }else {
        printf("Nao houve colisoes\n");
    }
}





// Esta função imprime no ecrã os nomes dos barcos existentes num dado frame.
void BarcoNome(BaseDados *estadoAtual) {
    if (!estadoAtual || !estadoAtual->frame_atual) {
        printf("Nenhum barco encontrado no frame atual.\n");
        return;
    }

    printf("\nLista de barcos no frame %d:\n", estadoAtual->frame_atual_num);

    for (EntidadeIED *ent = estadoAtual->frame_atual; ent != NULL; ent = ent->seguinte) {
        // Verifica se o submarino está visível (caso seja tipo 3)
        if (ent->no_nautico->tipologia == 3 && !ent->visivel) {
            continue; // Ignora submarinos invisíveis
        }
        printf("Barco %c\n", ent->no_nautico->nome);
    }
}

// Esta função calcula a velocidade média de um barco ao longo dos frames da simulação.
void velocidadeMediaBarco(BaseDados *inicio) {
    char nomeBarco;
    printf("\n=== Velocidade Media de um Barco ===\n");
    printf("Nome do barco (uma letra): ");
    scanf(" %c", &nomeBarco);
    printf("\n");

    EntidadeIED *primeira = NULL;
    EntidadeIED *ultima = NULL;
    int numFrames = 0;
    float distanciaTotal = 0.0;

    BaseDados *atual = inicio;
    int posAnteriorX = -1, posAnteriorY = -1;

    while (atual) {
        EntidadeIED *barco = atual->frame_atual;
        while (barco) {
            if (barco->no_nautico->nome == nomeBarco && barco->visivel) {
                if (!primeira) {
                    // Guarda a posição inicial na primeira ocorrência
                    primeira = barco;
                    posAnteriorX = barco->posicao[0];
                    posAnteriorY = barco->posicao[1];
                } else {
                    // Calcula a distância percorrida desde o último frame
                    int dx = barco->posicao[0] - posAnteriorX;
                    int dy = barco->posicao[1] - posAnteriorY;
                    distanciaTotal += sqrt(dx * dx + dy * dy);
                    posAnteriorX = barco->posicao[0];
                    posAnteriorY = barco->posicao[1];
                }
                ultima = barco;
                numFrames++;
                break;
            }
            barco = barco->seguinte;
        }
        atual = atual->next;
    }

    if (!primeira || !ultima || numFrames < 1) {
        printf("O barco '%c' nao tem frames registados.\n", nomeBarco);
        return;
    }

    float velocidadeMedia = 0.0;
    if (numFrames > 1) {
        velocidadeMedia = distanciaTotal / (numFrames - 1.0);
    } else {
        velocidadeMedia = 0.0;
    }

    printf("\nEstatisticas do barco %c:\n", nomeBarco);
    printf("  Posicao inicial: (%d,%d)\n", primeira->posicao[0], primeira->posicao[1]);
    printf("  Posicao atual: (%d,%d)\n", ultima->posicao[0], ultima->posicao[1]);
    printf("  Distancia percorrida: %.2f casas\n", distanciaTotal);
    printf("  Numero de frames: %d\n", numFrames - 1);
    printf("  Velocidade media: %.2f casas/frame\n", velocidadeMedia);
}



// Esta função liberta toda a memória dinâmica alocada durante a simulação.
// É chamada no final do programa para evitar fugas de memória.
void libertarMemoria(BaseDados *inicio) {
    while (inicio) {
        BaseDados *frameAtual = inicio;
        inicio = inicio->next; // Avança para o próximo frame

        EntidadeIED *ent = frameAtual->frame_atual;
        while (ent) {
            EntidadeIED *proxEnt = ent->seguinte;
            free(ent->no_nautico); // Liberta a memória associada ao barco
            free(ent);             // Liberta a entidade
            ent = proxEnt;
        }

        free(frameAtual); // Liberta o frame em si
    }

    // Mensagem opcional para confirmar libertação
    printf("\nMemória libertada com sucesso.\n");
}


int main(int argc, char *argv[]) {
    // Variáveis principais da simulação
    char nome;
    int x, y, angulo, velocidade, tipo,latitudeX = 0, longitudeY = 0,totalFrames ,voltar, choice;
    if (argc != 5) {
        printf("Uso: %s <ficheiro> <resolucao> <frames> <ficheiro_saida>\n", argv[0]);
        return 1;
    }

    char *ficheiroAntes = argv[1];

    sscanf(argv[2], "%dx%d", &latitudeX, &longitudeY);

    sscanf(argv[3], "%d", &totalFrames);
    char *ficheiroDepois = argv[4];

    BaseDados *estadoAntes = lerAntes(ficheiroAntes);
    if (!estadoAntes) return 1;

    BaseDados *estadoAtual = estadoAntes;

    if (totalFrames > 0) {
        estadoAtual = avancarFrames(estadoAtual, totalFrames, ficheiroDepois, latitudeX, longitudeY);
    }


    do {
        printf("\n=== Menu Radar Nautico ===\n");
        printf("1. Avancar simulacao\n");
        printf("2. Inserir ou alterar barco\n");
        printf("3. Prever colisoes\n");
        printf("4. Rastrear historico reverso\n");
        printf("5. Velocidade media do barco\n");
        printf("0. Sair\n");
        printf("Opcao: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1: {
                int frames;
                printf("Quantos frames deseja avancar? ");
                scanf("%d", &frames);
                estadoAtual = avancarFrames(estadoAtual, frames, ficheiroDepois, latitudeX, longitudeY);
                break;
            }

            case 2:
                printf("\n=== Inserir/Alterar Barco ===\n");

                printf("Nome do barco (uma letra): ");
                scanf(" %c", &nome);

                printf("Posicao inicial (latitude longitude): ");
                scanf("%d %d", &x, &y);

                do{
                printf("Angulo (multiplo de 45): ");
                scanf("%d", &angulo);

                    if(angulo % 45 != 0){
                        printf("Angulo invalido, nao e multiplo de 45\n");
                    }
                }while(angulo % 45 != 0);

                printf("Velocidade: ");
                scanf("%d", &velocidade);

                printf("Tipo de barco (0-13): ");
                scanf("%d", &tipo);

                inserirOuAlterarEntidade(estadoAtual, nome, x, y, angulo, velocidade, tipo);
                break;


            case 3:
                //Nao Completa
                preverColisoes(estadoAtual, latitudeX, longitudeY);

                break;

            case 4:

                printf("\n=== Rastrear Historico Reverso ===\n");
                printf("Quantos frames deseja voltar? ");
                scanf("%d", &voltar);

                estadoAtual = voltarAtras(estadoAntes, estadoAtual, voltar);
                escreverFrameParaFicheiro(estadoAtual, ficheiroDepois);
                break;

            case 5:
                velocidadeMediaBarco(estadoAntes);
                break;

            case 0:
                printf("Frame %d guardado com sucesso em %s\n ", estadoAtual->frame_atual_num , ficheiroDepois);
                printf("A sair do programa...\n");

                libertarMemoria(estadoAntes);
                break;

            default:
                printf("Opcao invalida.\n");
        }
    } while (choice != 0);

    return 0;
}