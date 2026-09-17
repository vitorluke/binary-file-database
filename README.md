##  Binary File Database  ##
    Projedo de Estrutura de Dados III para a manipulação de arquivos binários com base no projeto Internet Topology Zoo.
O programa permite a criação de uma arquivo binario com base nos dados do arquivo csv, inserção no arquivo binário, busca, remoção e atualização.

Autores:

    Murilo Antonio de Oliveira Amatti - NroUSP: 17150041
    Vitor Quirino Machado - NroUSP: 16904644

Funcionalidades:

    1 - create_table, criação de um arquivo binário com base num .csv.
        No terminal:
        "1 nomeDoArquivoCsv.csv nomeDoArquivoBinario.bin"

    2 - select_table, mostragem de todos os dados salvos no binário.
        No terminal:
        "2 nomeDoArquivoBinario.bin"

    3 - select_where, busca de dados com base num filtro de campos.
        No terminal:
        "3 nomeDoArquivoBinario.bin numeroDeFiltros
        numeroDeCampos nomeCampo valorCampo ..."

    4 - busca_rrn, busca direta utilizando o RRN do registro.
        No terminal:
        "4 nomeDoArquivoBinario.bin rrn"

    5 - delete_register, remoção lógica de um registro com empilhamento de RRN.
        No terminal:
        "5 nomeDoArquivoBinario.bin numeroDeFiltro
        numeroDeCampos nomeCampo valorCampo ..."

    6 - insert_into, inserção de um registro com reaproveitamento da pilha de RRN.
        No terminal:
        "6 nomeDoArquivoBinario.bin numeroDeRegistroAdicionados
        idPoPsDoRegistro idPoPsConectadoDoRegistro velocidadeDoRegistro unidadeMedidaDoRegistro ..."

    7 - update, atualização de campos em registros filtrados.
        No terminal:
        "7 nomeDoArquivoBinario.bin numeroDeFiltro
        numeroDeCamposDoFiltro nomeCampo valorCampo ...
        numeroDeCamposASeremAtualizados nomeCampo valorCampo ..."

Estrutura de arquivos:

    main.c - chamadas das funcionalidades.
    delete_insert.c - funcionalidades delete_register, insert_into e update.
    rrn.c - funcionalidade busca_rrn.
    select.c - funcionalidades select_table e select_where.
    table.c - funcionalidade create_table.
    func.h - declaração das funcionalidades e estrutura do cabeçalho e registros.
    fornecidas.c - funções fornecidas.
    fornecidas.h - declaração das funções fornecidas.
    Makefile - necessário para compilar no terminal.

Como compilar/executar:

    No terminal digite "make all" para compilar e digite "make run" para executar.

Ambiente Linux.