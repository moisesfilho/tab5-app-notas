# tab5-app-notas

Aplicativo de Bloco de Notas para o sistema operacional **Tab5 OS** (M5Stack Tab5 / ESP32-P4), desacoplado e compilado para execução isolada em WebAssembly (WAMR).

## Características

- Interface minimalista e responsiva integrada ao tema visual do Tab5 OS
- Criação, edição e salvamento de arquivos de texto em sandbox (`/sdcard/data/com.tab5.notas/`)
- Integração nativa com a barra de título do aplicativo (`App Bar`) e botões de ação
- Associação automática de arquivos com as extensões `.txt`, `.cfg` e `.md`
- Suporte ao Teclado Virtual nativo e teclados físicos Bluetooth HID

## Como Compilar e Gerar o Pacote

```bash
# Executa o script de empacotamento
./tools/build.sh
```

O pacote resultante `com.tab5.notas.tab5pkg` será criado na pasta `dist/` e pode ser instalado diretamente no dispositivo via Cartão SD (`/sdcard/apps/`) ou através do aplicativo Gerenciador de Armazenamento.

## Licença

MIT License.
