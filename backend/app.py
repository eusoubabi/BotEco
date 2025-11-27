from flask import Flask, jsonify, request
from flask_cors import CORS
import requests

app = Flask(__name__)
CORS(app)

# ===========================
# CONFIGURAÇÃO DO ESP32
# ===========================

ESP_IP = "10.45.225.90"   # <-- DEFINIR IP DETECTADO
MODO_SIMULADO = False     # false = usa o ESP real


# ===========================
# FUNÇÕES DE COMUNICAÇÃO
# ===========================

def chamar_esp32(endpoint):
    """Tenta acessar o ESP32. Retorna True se funcionar."""
    url = f"http://{ESP_IP}{endpoint}"

    try:
        r = requests.get(url, timeout=3)
        return r.ok
    except:
        return False


def status_esp():
    """Verifica se o ESP está online."""
    try:
        r = requests.get(f"http://{ESP_IP}/api/status_rota", timeout=3)
        if r.ok:
            return True
    except:
        pass

    return False


# ===========================
# ROTAS API
# ===========================

@app.route("/api/info")
def api_info():
    return jsonify({
        "esp_ip": ESP_IP,
        "online": status_esp()
    })


@app.route("/api/rota/<rota>", methods=["POST"])
def api_rota(rota):
    ok = chamar_esp32(f"/api/rota/{rota}")
    if ok:
        return jsonify({"sucesso": True})
    return jsonify({"sucesso": False}), 500


@app.route("/api/parar", methods=["POST"])
def api_parar():
    chamar_esp32("/api/parar")
    return jsonify({"sucesso": True})


@app.route("/api/status_rota")
def api_status():
    try:
        r = requests.get(f"http://{ESP_IP}/api/status_rota", timeout=3)
        return r.json()
    except:
        return jsonify({"estado": "erro"})


# ===========================
# MAIN
# ===========================

if __name__ == "__main__":
    print("Backend rodando em http://127.0.0.1:5000")
    app.run(debug=True)
