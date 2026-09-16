/*
  ============================================================
  GIGA 1 - 12 LDRs
  ============================================================

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

  Comunicação:
  GIGA 1 TX1 -> GIGA 2 RX1
  GIGA 1 GND -> GIGA 2 GND

  Sem multiplexador.
*/

// ============================================================
// QUANTIDADE DE LDRs
// ============================================================

#define NUM_LDR 12

// ============================================================
// VETOR DOS VALORES DOS LDRs
// ============================================================

int ldr[NUM_LDR];

// ============================================================
// SETUP
// ============================================================

void setup() {

  // Serial para o computador
  Serial.begin(115200);

  // Serial1 para comunicação com o GIGA 2
  Serial1.begin(115200);

  Serial.println();
  Serial.println("======================================");
  Serial.println("          GIGA 1 - 12 LDRs");
  Serial.println("======================================");
  Serial.println("LDR1  -> A0");
  Serial.println("LDR2  -> A1");
  Serial.println("LDR3  -> A2");
  Serial.println("LDR4  -> A3");
  Serial.println("LDR5  -> A4");
  Serial.println("LDR6  -> A5");
  Serial.println("LDR7  -> A6");
  Serial.println("LDR8  -> A7");
  Serial.println("LDR9  -> A8");
  Serial.println("LDR10 -> A9");
  Serial.println("LDR11 -> A10");
  Serial.println("LDR12 -> A11");
  Serial.println();
  Serial.println("Sem multiplexador.");
  Serial.println("Iniciando leitura...");
  Serial.println();
}

// ============================================================
// LOOP
// ============================================================

void loop() {

  // Ler os 12 LDRs
  lerLDR();

  // Mostrar os valores
  mostrarLDR();

  // Enviar para o GIGA 2
  enviarLDR();

  delay(50);
}

// ============================================================
// LER OS 12 LDRs
// ============================================================

void lerLDR() {

  ldr[0]  = analogRead(A0);
  ldr[1]  = analogRead(A1);
  ldr[2]  = analogRead(A2);
  ldr[3]  = analogRead(A3);

  ldr[4]  = analogRead(A4);
  ldr[5]  = analogRead(A5);
  ldr[6]  = analogRead(A6);
  ldr[7]  = analogRead(A7);

  ldr[8]  = analogRead(A8);
  ldr[9]  = analogRead(A9);
  ldr[10] = analogRead(A10);
  ldr[11] = analogRead(A11);
}

// ============================================================
// MOSTRAR OS LDRs NO MONITOR SERIAL
// ============================================================

void mostrarLDR() {

  Serial.print("L1=");
  Serial.print(ldr[0]);

  Serial.print(" | L2=");
  Serial.print(ldr[1]);

  Serial.print(" | L3=");
  Serial.print(ldr[2]);

  Serial.print(" | L4=");
  Serial.print(ldr[3]);

  Serial.print(" | L5=");
  Serial.print(ldr[4]);

  Serial.print(" | L6=");
  Serial.print(ldr[5]);

  Serial.print(" | L7=");
  Serial.print(ldr[6]);

  Serial.print(" | L8=");
  Serial.print(ldr[7]);

  Serial.print(" | L9=");
  Serial.print(ldr[8]);

  Serial.print(" | L10=");
  Serial.print(ldr[9]);

  Serial.print(" | L11=");
  Serial.print(ldr[10]);

  Serial.print(" | L12=");
  Serial.println(ldr[11]);
}

// ============================================================
// ENVIAR OS LDRs PARA O GIGA 2
// ============================================================

void enviarLDR() {

  /*
    Formato enviado:

    <L1,L2,L3,L4,L5,L6,L7,L8,L9,L10,L11,L12>

    Exemplo:

    <1200,1300,1400,1500,900,800,700,600,1100,1200,1300,1400>
  */

  Serial1.print("<");

  Serial1.print(ldr[0]);
  Serial1.print(",");

  Serial1.print(ldr[1]);
  Serial1.print(",");

  Serial1.print(ldr[2]);
  Serial1.print(",");

  Serial1.print(ldr[3]);
  Serial1.print(",");

  Serial1.print(ldr[4]);
  Serial1.print(",");

  Serial1.print(ldr[5]);
  Serial1.print(",");

  Serial1.print(ldr[6]);
  Serial1.print(",");

  Serial1.print(ldr[7]);
  Serial1.print(",");

  Serial1.print(ldr[8]);
  Serial1.print(",");

  Serial1.print(ldr[9]);
  Serial1.print(",");

  Serial1.print(ldr[10]);
  Serial1.print(",");

  Serial1.print(ldr[11]);

  Serial1.println(">");
}
