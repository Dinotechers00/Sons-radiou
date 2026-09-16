/*
  ============================================================
  ROBO SOCCER
  10 SENSORES IR + MOSFET + PONTES H
  + RECEPCAO DE 16 LDRs PELO GIGA 1
  ============================================================

  NOVA LOGICA:

  - Quando a bola estiver na frente -> segue reto.
  - Quando estiver em FRENTE DIREITA (pino 30) -> segue reto.
  - Quando estiver em FRENTE ESQUERDA (pino 31) -> segue reto.
  - Quando estiver em qualquer outra direcao lateral -> gira.
  - Quando voltar para FRENTE -> segue reto novamente.

  COMUNICACAO DOS LDRs:

  GIGA 1 TX1 -> GIGA 2 RX1
  GIGA 1 GND -> GIGA 2 GND

  O GIGA 1 envia:

  <LDR0,LDR1,LDR2,...,LDR15>

  O GIGA 2 recebe e armazena em:

  ldr[0] ... ldr[15]

  NAO USA MULTIPLEXADOR.
*/


// ============================================================
// PINOS DOS MOTORES
// ============================================================

#define in_1_ph_1 25
#define in_2_ph_1 24
#define in_3_ph_1 23
#define in_4_ph_1 22

#define in_1_ph_2 47
#define in_2_ph_2 46
#define in_3_ph_2 49
#define in_4_ph_2 48

#define pwm_2 2
#define pwm_1 3
#define pwm_4 4
#define pwm_3 5


// ============================================================
// SENTIDOS
// ============================================================

#define HORARIO 1
#define ANTI_HORARIO -1
#define PARADO 0


// ============================================================
// DIRECOES DO INFRAVERMELHO
// ============================================================

enum DirecaoIr {

  IR_FRENTE,

  IR_FRENTE_DIREITA,
  IR_DIREITA,
  IR_TRAS_DIREITA,
  IR_TRAS,
  IR_TRAS_ESQUERDA,
  IR_ESQUERDA,
  IR_FRENTE_ESQUERDA,

  // Pino 30
  IR_FRENTE_DIREITA_30,

  // Pino 31
  IR_FRENTE_ESQUERDA_31,

  IR_NAO_DETECTADA
};


// ============================================================
// CONFIGURACAO DOS 10 SENSORES IR
// ============================================================

constexpr uint8_t SENSOR_COUNT = 10;

constexpr uint8_t SENSOR_PINS[SENSOR_COUNT] = {

  40,  // Sensor 1 - FRENTE
  39,  // Sensor 2 - FRENTE_DIREITA
  38,  // Sensor 3 - DIREITA
  37,  // Sensor 4 - TRAS_DIREITA
  36,  // Sensor 5 - TRAS
  35,  // Sensor 6 - TRAS_ESQUERDA
  34,  // Sensor 7 - ESQUERDA
  33,  // Sensor 8 - FRENTE_ESQUERDA

  30,  // Sensor 9 - FRENTE_DIREITA
  31   // Sensor 10 - FRENTE_ESQUERDA
};


// ============================================================
// MOSFET QUE ALIMENTA OS SENSORES
// ============================================================

constexpr uint8_t SENSOR_POWER_PIN = 10;


// ============================================================
// DIRECOES DOS SENSORES
// ============================================================

const DirecaoIr SENSOR_DIRECTIONS[SENSOR_COUNT] = {

  IR_FRENTE,
  IR_FRENTE_DIREITA,
  IR_DIREITA,
  IR_TRAS_DIREITA,
  IR_TRAS,
  IR_TRAS_ESQUERDA,
  IR_ESQUERDA,
  IR_FRENTE_ESQUERDA,

  IR_FRENTE_DIREITA_30,
  IR_FRENTE_ESQUERDA_31
};


// ============================================================
// NOMES DAS DIRECOES
// ============================================================

const char *const directionName[] = {

  "FRENTE",
  "FRENTE_DIREITA",
  "DIREITA",
  "TRAS_DIREITA",
  "TRAS",
  "TRAS_ESQUERDA",
  "ESQUERDA",
  "FRENTE_ESQUERDA",

  "FRENTE_DIREITA",
  "FRENTE_ESQUERDA",

  "NAO_DETECTADA"
};


// ============================================================
// CONFIGURACAO DOS IR
// ============================================================

constexpr uint16_t WINDOW_US = 25000;
constexpr uint16_t DETECT_MIN_LOW_US = 800;

constexpr uint8_t POWER_OFF_MS = 10;
constexpr uint8_t POWER_SETTLE_MS = 15;

constexpr uint16_t SWITCH_MARGIN_US = 700;
constexpr uint8_t SWITCH_CONFIRMATIONS = 2;


// ============================================================
// VARIAVEIS DOS IR
// ============================================================

uint16_t readings[SENSOR_COUNT];
uint16_t filteredReadings[SENSOR_COUNT];

bool filterInitialized = false;

uint8_t lastDirection = 0;
uint8_t pendingDirection = 0;
uint8_t pendingCount = 0;


// ============================================================
// LDRs
// ============================================================

#define NUM_LDR 16

int ldr[NUM_LDR];

String dadosLDR = "";

bool recebendoLDR = false;


// ============================================================
// CONFIGURACOES DO ROBO
// ============================================================

const bool DEBUG_SERIAL_ATIVO = true;

int velocidadeFrente = 80;
int velocidadeGiro = 68;
int velocidadeBusca = 80;


// ============================================================
// ESTADO ATUAL DO IR
// ============================================================

DirecaoIr direcaoIrAtual = IR_NAO_DETECTADA;

uint8_t sensorIrAtual = 0;

unsigned long ultimoPrintSerial = 0;


// ============================================================
// SETUP
// ============================================================

void setup() {

  // ----------------------------------------------------------
  // MONITOR SERIAL
  // ----------------------------------------------------------

  if (DEBUG_SERIAL_ATIVO) {
    Serial.begin(115200);
  }


  // ----------------------------------------------------------
  // COMUNICACAO COM GIGA 1
  // ----------------------------------------------------------

  Serial1.begin(115200);


  // ----------------------------------------------------------
  // PINOS DA PONTE H
  // ----------------------------------------------------------

  pinMode(in_1_ph_1, OUTPUT);
  pinMode(in_2_ph_1, OUTPUT);
  pinMode(in_3_ph_1, OUTPUT);
  pinMode(in_4_ph_1, OUTPUT);

  pinMode(in_1_ph_2, OUTPUT);
  pinMode(in_2_ph_2, OUTPUT);
  pinMode(in_3_ph_2, OUTPUT);
  pinMode(in_4_ph_2, OUTPUT);

  pinMode(pwm_1, OUTPUT);
  pinMode(pwm_2, OUTPUT);
  pinMode(pwm_3, OUTPUT);
  pinMode(pwm_4, OUTPUT);


  // ----------------------------------------------------------
  // SENSORES IR
  // ----------------------------------------------------------

  for (uint8_t i = 0; i < SENSOR_COUNT; ++i) {
    pinMode(SENSOR_PINS[i], INPUT);
  }


  // ----------------------------------------------------------
  // MOSFET
  // ----------------------------------------------------------

  pinMode(SENSOR_POWER_PIN, OUTPUT);

  digitalWrite(SENSOR_POWER_PIN, LOW);


  // ----------------------------------------------------------
  // PARA O ROBO
  // ----------------------------------------------------------

  stop_robot();

  delay(500);


  // ----------------------------------------------------------
  // MENSAGEM INICIAL
  // ----------------------------------------------------------

  if (DEBUG_SERIAL_ATIVO) {

    Serial.println();
    Serial.println("==========================================");
    Serial.println("ROBO INICIADO");
    Serial.println("10 SENSORES IR + MOSFET");
    Serial.println("16 LDRs RECEBIDOS PELO GIGA 1");
    Serial.println("MODO: GIRA -> ALINHA -> SEGUE RETO");
    Serial.println("D30 = FRENTE DIREITA");
    Serial.println("D31 = FRENTE ESQUERDA");
    Serial.println("==========================================");
  }
}


// ============================================================
// LOOP
// ============================================================

void loop() {

  // ----------------------------------------------------------
  // RECEBE OS LDRs DO GIGA 1
  // ----------------------------------------------------------

  receberLDR();


  // ----------------------------------------------------------
  // LE IR
  // ----------------------------------------------------------

  ir_reader();


  // ----------------------------------------------------------
  // MOSTRA INFORMACOES
  // ----------------------------------------------------------

  monitor_serial();


  // ----------------------------------------------------------
  // CONTROLA O ROBO
  // ----------------------------------------------------------

  seguir_bola();


  delay(10);
}


// ============================================================
// RECEBER LDRs DO GIGA 1
// ============================================================

void receberLDR() {

  while (Serial1.available()) {

    char c = Serial1.read();


    // --------------------------------------------------------
    // INICIO DO PACOTE
    // --------------------------------------------------------

    if (c == '<') {

      dadosLDR = "";

      recebendoLDR = true;

      continue;
    }


    // --------------------------------------------------------
    // FINAL DO PACOTE
    // --------------------------------------------------------

    if (c == '>') {

      if (recebendoLDR) {

        receberValoresLDR();

        recebendoLDR = false;
      }

      continue;
    }


    // --------------------------------------------------------
    // GUARDA OS DADOS
    // --------------------------------------------------------

    if (recebendoLDR) {

      dadosLDR += c;
    }
  }
}


// ============================================================
// CONVERTER OS 16 VALORES RECEBIDOS
// ============================================================

void receberValoresLDR() {

  int indice = 0;

  int inicio = 0;


  for (int i = 0; i <= dadosLDR.length(); i++) {

    if (i == dadosLDR.length() ||
        dadosLDR.charAt(i) == ',') {

      if (indice < NUM_LDR) {

        String valor =
          dadosLDR.substring(inicio, i);

        ldr[indice] = valor.toInt();

        indice++;
      }

      inicio = i + 1;
    }
  }
}


// ============================================================
// LEITURA PRINCIPAL DOS IR
// ============================================================

void ir_reader() {

  // ----------------------------------------------------------
  // DESLIGA OS SENSORES
  // ----------------------------------------------------------

  digitalWrite(SENSOR_POWER_PIN, LOW);

  delay(POWER_OFF_MS);


  // ----------------------------------------------------------
  // LIGA OS SENSORES
  // ----------------------------------------------------------

  digitalWrite(SENSOR_POWER_PIN, HIGH);

  delay(POWER_SETTLE_MS);


  // ----------------------------------------------------------
  // FAZ A LEITURA
  // ----------------------------------------------------------

  readSensors();


  // ----------------------------------------------------------
  // DESLIGA APOS A LEITURA
  // ----------------------------------------------------------

  digitalWrite(SENSOR_POWER_PIN, LOW);


  // ----------------------------------------------------------
  // FILTRO
  // ----------------------------------------------------------

  updateFilter();


  // ----------------------------------------------------------
  // DETECCAO
  // ----------------------------------------------------------

  if (ballDetected()) {

    uint8_t index = getStableDirection();

    sensorIrAtual = index + 1;

    direcaoIrAtual = SENSOR_DIRECTIONS[index];
  }

  else {

    sensorIrAtual = 0;

    direcaoIrAtual = IR_NAO_DETECTADA;
  }
}


// ============================================================
// LEITURA DOS SENSORES
// ============================================================

void readSensors() {

  for (uint8_t i = 0; i < SENSOR_COUNT; ++i) {

    readings[i] = 0;
  }


  const uint32_t start = micros();

  uint32_t previous = start;


  while ((uint32_t)(micros() - start) < WINDOW_US) {

    const uint32_t now = micros();

    const uint16_t elapsed =
      (uint16_t)(now - previous);

    previous = now;


    for (uint8_t i = 0; i < SENSOR_COUNT; ++i) {

      /*
        EK8460 / VS1838B:

        LOW significa que existe IR modulado
        sendo recebido.
      */

      if (digitalRead(SENSOR_PINS[i]) == LOW) {

        readings[i] += elapsed;
      }
    }
  }
}


// ============================================================
// VERIFICA SE A BOLA FOI DETECTADA
// ============================================================

bool ballDetected() {

  for (uint8_t i = 0; i < SENSOR_COUNT; ++i) {

    if (readings[i] >= DETECT_MIN_LOW_US) {

      return true;
    }
  }

  return false;
}


// ============================================================
// FILTRO EXPONENCIAL
// ============================================================

void updateFilter() {

  for (uint8_t i = 0; i < SENSOR_COUNT; ++i) {

    if (!filterInitialized) {

      filteredReadings[i] = readings[i];
    }

    else {

      /*
        75% leitura anterior
        25% leitura atual
      */

      filteredReadings[i] =
        (filteredReadings[i] * 3UL + readings[i]) / 4;
    }
  }

  filterInitialized = true;
}


// ============================================================
// ENCONTRA SENSOR MAIS FORTE
// ============================================================

uint8_t getFilteredStrongestDirection() {

  uint8_t strongest = lastDirection;


  for (uint8_t i = 0; i < SENSOR_COUNT; ++i) {

    if (filteredReadings[i] >
        filteredReadings[strongest]) {

      strongest = i;
    }
  }


  return strongest;
}


// ============================================================
// ESTABILIZA A DIRECAO
// ============================================================

uint8_t getStableDirection() {

  const uint8_t candidate =
    getFilteredStrongestDirection();


  // ----------------------------------------------------------
  // MESMA DIRECAO
  // ----------------------------------------------------------

  if (candidate == lastDirection) {

    pendingCount = 0;

    return lastDirection;
  }


  // ----------------------------------------------------------
  // PRECISA SER CLARAMENTE MAIS FORTE
  // ----------------------------------------------------------

  if (
    filteredReadings[candidate] <
    filteredReadings[lastDirection] + SWITCH_MARGIN_US
  ) {

    pendingCount = 0;

    return lastDirection;
  }


  // ----------------------------------------------------------
  // CONFIRMACAO
  // ----------------------------------------------------------

  if (candidate == pendingDirection) {

    ++pendingCount;
  }

  else {

    pendingDirection = candidate;

    pendingCount = 1;
  }


  // ----------------------------------------------------------
  // CONFIRMA NOVA DIRECAO
  // ----------------------------------------------------------

  if (pendingCount >= SWITCH_CONFIRMATIONS) {

    lastDirection = candidate;

    pendingCount = 0;
  }


  return lastDirection;
}


// ============================================================
// SEGUE A BOLA
// ============================================================

void seguir_bola() {

  switch (direcaoIrAtual) {


    // --------------------------------------------------------
    // BOLA NA FRENTE
    // --------------------------------------------------------

    case IR_FRENTE:

      Serial.println(
        ">>> BOLA FRENTE -> SEGUE RETO"
      );

      move_foward(velocidadeFrente);

      break;


    // --------------------------------------------------------
    // BOLA FRENTE DIREITA
    // PINO 39
    // --------------------------------------------------------

    case IR_FRENTE_DIREITA:

      Serial.println(
        ">>> BOLA FRENTE DIREITA -> FRENTE"
      );

      move_foward(velocidadeFrente);

      break;


    // --------------------------------------------------------
    // BOLA DIREITA
    // --------------------------------------------------------

    case IR_DIREITA:

      Serial.println(
        ">>> BOLA DIREITA -> GIRA DIREITA"
      );

      right_rotation(velocidadeGiro);

      break;


    // --------------------------------------------------------
    // BOLA TRAS DIREITA
    // --------------------------------------------------------

    case IR_TRAS_DIREITA:

      Serial.println(
        ">>> BOLA TRAS DIREITA -> GIRA DIREITA"
      );

      right_rotation(velocidadeGiro);

      break;


    // --------------------------------------------------------
    // BOLA ATRAS
    // --------------------------------------------------------

    case IR_TRAS:

      Serial.println(
        ">>> BOLA TRAS -> GIRA DIREITA"
      );

      right_rotation(velocidadeGiro);

      break;


    // --------------------------------------------------------
    // BOLA TRAS ESQUERDA
    // --------------------------------------------------------

    case IR_TRAS_ESQUERDA:

      Serial.println(
        ">>> BOLA TRAS ESQUERDA -> GIRA ESQUERDA"
      );

      left_rotation(velocidadeGiro);

      break;


    // --------------------------------------------------------
    // BOLA ESQUERDA
    // --------------------------------------------------------

    case IR_ESQUERDA:

      Serial.println(
        ">>> BOLA ESQUERDA -> GIRA ESQUERDA"
      );

      left_rotation(velocidadeGiro);

      break;


    // --------------------------------------------------------
    // BOLA FRENTE ESQUERDA
    // PINO 33
    // --------------------------------------------------------

    case IR_FRENTE_ESQUERDA:

      Serial.println(
        ">>> BOLA FRENTE ESQUERDA -> FRENTE"
      );

      move_foward(velocidadeFrente);

      break;


    // --------------------------------------------------------
    // NOVO SENSOR - PINO 30
    // FRENTE DIREITA
    // --------------------------------------------------------

    case IR_FRENTE_DIREITA_30:

      Serial.println(
        ">>> BOLA FRENTE DIREITA (PINO 30) -> SEGUE RETO"
      );

      move_foward(velocidadeFrente);

      break;


    // --------------------------------------------------------
    // NOVO SENSOR - PINO 31
    // FRENTE ESQUERDA
    // --------------------------------------------------------

    case IR_FRENTE_ESQUERDA_31:

      Serial.println(
        ">>> BOLA FRENTE ESQUERDA (PINO 31) -> SEGUE RETO"
      );

      move_foward(velocidadeFrente);

      break;


    // --------------------------------------------------------
    // NENHUMA BOLA
    // --------------------------------------------------------

    case IR_NAO_DETECTADA:

    default:

      Serial.println(
        ">>> NENHUMA BOLA -> BUSCANDO"
      );

      right_rotation(velocidadeBusca);

      break;
  }
}


// ============================================================
// MONITOR SERIAL
// ============================================================

void monitor_serial() {

  if (!DEBUG_SERIAL_ATIVO) {
    return;
  }


  if (millis() - ultimoPrintSerial < 100) {
    return;
  }


  ultimoPrintSerial = millis();


  // ----------------------------------------------------------
  // LEITURAS BRUTAS DOS IR
  // ----------------------------------------------------------

  Serial.print("RAW: ");

  for (uint8_t i = 0; i < SENSOR_COUNT; ++i) {

    Serial.print(readings[i]);

    if (i < SENSOR_COUNT - 1) {
      Serial.print(",");
    }
  }


  // ----------------------------------------------------------
  // LEITURAS FILTRADAS
  // ----------------------------------------------------------

  Serial.print(" | FILTRO: ");

  for (uint8_t i = 0; i < SENSOR_COUNT; ++i) {

    Serial.print(filteredReadings[i]);

    if (i < SENSOR_COUNT - 1) {
      Serial.print(",");
    }
  }


  // ----------------------------------------------------------
  // SENSOR
  // ----------------------------------------------------------

  Serial.print(" | Sensor: ");

  if (sensorIrAtual == 0) {

    Serial.print("NAO_DETECTADO");
  }

  else {

    Serial.print(sensorIrAtual);
  }


  // ----------------------------------------------------------
  // DIRECAO
  // ----------------------------------------------------------

  Serial.print(" | Direcao: ");

  Serial.print(
    directionName[direcaoIrAtual]
  );


  // ----------------------------------------------------------
  // LDRs
  // ----------------------------------------------------------

  Serial.print(" | LDR: ");

  for (uint8_t i = 0; i < NUM_LDR; ++i) {

    Serial.print(ldr[i]);

    if (i < NUM_LDR - 1) {
      Serial.print(",");
    }
  }


  Serial.println();
}


// ============================================================
// MOTOR 1
// ============================================================

void motor_1(int sentido, int speed) {

  if (sentido == HORARIO) {

    digitalWrite(in_3_ph_1, HIGH);
    digitalWrite(in_4_ph_1, LOW);
  }

  else if (sentido == ANTI_HORARIO) {

    digitalWrite(in_3_ph_1, LOW);
    digitalWrite(in_4_ph_1, HIGH);
  }

  else {

    digitalWrite(in_3_ph_1, LOW);
    digitalWrite(in_4_ph_1, LOW);
  }

  analogWrite(pwm_1, speed);
}


// ============================================================
// MOTOR 2
// ============================================================

void motor_2(int sentido, int speed) {

  if (sentido == HORARIO) {

    digitalWrite(in_1_ph_1, HIGH);
    digitalWrite(in_2_ph_1, LOW);
  }

  else if (sentido == ANTI_HORARIO) {

    digitalWrite(in_1_ph_1, LOW);
    digitalWrite(in_2_ph_1, HIGH);
  }

  else {

    digitalWrite(in_1_ph_1, LOW);
    digitalWrite(in_2_ph_1, LOW);
  }

  analogWrite(pwm_2, speed);
}


// ============================================================
// MOTOR 3
// ============================================================

void motor_3(int sentido, int speed) {

  if (sentido == HORARIO) {

    digitalWrite(in_3_ph_2, HIGH);
    digitalWrite(in_4_ph_2, LOW);
  }

  else if (sentido == ANTI_HORARIO) {

    digitalWrite(in_3_ph_2, LOW);
    digitalWrite(in_4_ph_2, HIGH);
  }

  else {

    digitalWrite(in_3_ph_2, LOW);
    digitalWrite(in_4_ph_2, LOW);
  }

  analogWrite(pwm_3, speed);
}


// ============================================================
// MOTOR 4
// ============================================================

void motor_4(int sentido, int speed) {

  if (sentido == HORARIO) {

    digitalWrite(in_1_ph_2, LOW);
    digitalWrite(in_2_ph_2, HIGH);
  }

  else if (sentido == ANTI_HORARIO) {

    digitalWrite(in_1_ph_2, HIGH);
    digitalWrite(in_2_ph_2, LOW);
  }

  else {

    digitalWrite(in_1_ph_2, LOW);
    digitalWrite(in_2_ph_2, LOW);
  }

  analogWrite(pwm_4, speed);
}


// ============================================================
// FRENTE
// ============================================================

void move_foward(int v) {

  motor_1(HORARIO, v);
  motor_2(HORARIO, v);
  motor_3(ANTI_HORARIO, v);
  motor_4(HORARIO, v);
}


// ============================================================
// TRAS
// ============================================================

void move_back(int v) {

  motor_1(ANTI_HORARIO, v);
  motor_2(ANTI_HORARIO, v);
  motor_3(HORARIO, v);
  motor_4(ANTI_HORARIO, v);
}


// ============================================================
// ROTACAO ESQUERDA
// ============================================================

void left_rotation(int v) {

  motor_1(HORARIO, v);
  motor_2(ANTI_HORARIO, v);
  motor_3(HORARIO, v);
  motor_4(HORARIO, v);
}


// ============================================================
// ROTACAO DIREITA
// ============================================================

void right_rotation(int v) {

  motor_1(ANTI_HORARIO, v);
  motor_2(HORARIO, v);
  motor_3(ANTI_HORARIO, v);
  motor_4(ANTI_HORARIO, v);
}


// ============================================================
// PARAR ROBO
// ============================================================

void stop_robot() {

  motor_1(PARADO, 0);
  motor_2(PARADO, 0);
  motor_3(PARADO, 0);
  motor_4(PARADO, 0);
}
