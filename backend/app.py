from flask import Flask, render_template, jsonify, request
import requests

app = Flask(__name__)

# IP do ESP32 (quando estiver com bateria)
ESP_IP = "192.168.4.1"

# Quando o ESP não estiver ligado, ativamos o modo simulado
MODO_SIMULADO = False

estado_simulado = "parado"


@app.route("/")
def index():
    return {"status": "backend ok"}


def chamar_esp32(endpoint):
    global MODO_SIMULADO, estado_simulado

    if MODO_SIMULADO:
        print(f"[SIMULADO] Chamando: {endpoint}")

        # Respostas simuladas
        if "a_ida" in endpoint:
            estado_simulado = "andando"
        if "a_volta" in endpoint:
            estado_simulado = "andando"
        if "b_ida" in endpoint:
            estado_simulado = "andando"
        if "b_volta" in endpoint:
            estado_simulado = "andando"
        if "parar" in endpoint:
            estado_simulado = "parado"

        return True

    # Modo real
    try:
        r = requests.get(f"http://{ESP_IP}{endpoint}", timeout=3)
        return r.ok
    except:
        return False


@app.route("/api/rota/<rota>", methods=["POST"])
def api_rota(rota):
    if chamar_esp32(f"/api/rota/{rota}"):
        return jsonify({"sucesso": True})
    return jsonify({"sucesso": False}), 500


@app.route("/api/parar", methods=["POST"])
def api_parar():
    chamar_esp32("/api/parar")
    return jsonify({"sucesso": True})


@app.route("/api/status_rota")
def api_status():
    global estado_simulado

    if MODO_SIMULADO:
        # Simula finalização após alguns ciclos
        import random
        if estado_simulado == "andando" and random.random() < 0.25:
            estado_simulado = "chegou"
        return jsonify({"estado": estado_simulado})

    # Modo real
    try:
        r = requests.get(f"http://{ESP_IP}/api/status_rota", timeout=3)
        return r.json()
    except:
        return jsonify({"estado": "erro"})


if __name__ == "__main__":
    print("Backend rodando em http://127.0.0.1:5000")
    app.run(debug=True)
