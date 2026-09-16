/*
  ============================================================
  GIGA 1 - 16 LDRs
  ============================================================

  O GIGA 1 é responsável por:

  - Ler os 16 LDRs diretamente
  - Enviar os valores para o GIGA 2 pela Serial1

  LIGAÇÃO DOS LDRs:

    LDR 1  -> A0
    LDR 2  -> A1
    LDR 3  -> A2
    LDR 4  -> A3
    LDR 5  -> A4
    LDR 6  -> A5
    LDR 7  -> A6
    LDR 8  -> A7
    LDR 9  -> A8
    LDR 10 -> A9
    LDR 11 -> A10
    LDR 12 -> A11
    LDR 13 -> A12
    LDR 14 -> A13
    LDR 15 -> A14
    LDR 16 -> A15

  COMUNICAÇÃO:

    GIGA 1 TX1 -> GIGA 2 RX1
    GIGA 1 GND -> GIGA 2 GND

  IMPORTANTE:
    O GIGA trabalha com 3,3 V.
*/

// ============================================================
// CONFIGURAÇÃO
// ============================================================

#define NUM_LDR 16

const int pinosLDR[NUM_LDR] = {
  A0, A1, A2, A3,
  A4, A5, A6, A7,
  A8, A9, A10, A11,
  A12, A13, A14, A15
};

// ============================================================
// VETOR DOS LDRs
// ============================================================

int ldr[NUM_LDR];

// ============================================================
// SETUP
// ============================================================

void setup() {

  // Monitor Serial do GIGA 1
  Serial.begin(115200);

  // Comunicação com o GIGA 2
  Serial1.begin(115200);

  // Configura os LDRs
  for (int i = 0; i < NUM_LDR; i++) {
    pinMode(pinosLDR[i], INPUT);
  }

  Serial.println();
  Serial.println("================================");
  Serial.println("GIGA 1 - SISTEMA DOS 16 LDRs");
  Serial.println("================================");
  Serial.println("Sem multiplexador");
  Serial.println("Iniciando leitura...");
}

// ============================================================
// LOOP
// ============================================================

void loop() {

  // 1. Lê os 16 LDRs
  lerLDR();

  // 2. Mostra no computador
  mostrarLDR();

  // 3. Envia para o GIGA 2
  enviarLDR();

  delay(50);
}

// ============================================================
// LER OS 16 LDRs
// ============================================================

void lerLDR() {

  for (int i = 0; i < NUM_LDR; i++) {

    ldr[i] = analogRead(pinosLDR[i]);

  }
}

// ============================================================
// MOSTRAR OS LDRs NO MONITOR SERIAL
// ============================================================

void mostrarLDR() {

  Serial.print("LDR: ");

  for (int i = 0; i < NUM_LDR; i++) {

    Serial.print(ldr[i]);

    if (i < NUM_LDR - 1) {
      Serial.print(" | ");
    }
  }

  Serial.println();
}

// ============================================================
// ENVIAR OS LDRs PARA O GIGA 2
// ============================================================

void enviarLDR() {

  /*
    O GIGA 1 envia:

    <LDR0,LDR1,LDR2,...,LDR15>

    Exemplo:

    <123,456,789,234,567,890,123,456,
     789,234,567,890,123,456,789,234>
  */

  Serial1.print("<");

  for (int i = 0; i < NUM_LDR; i++) {

    Serial1.print(ldr[i]);

    if (i < NUM_LDR - 1) {
      Serial1.print(",");
    }
  }

  Serial1.println(">");
}
