import { useEffect, useState } from "react";

const API_URL = import.meta.env.VITE_API_URL || "http://127.0.0.1:5000";

export default function Dashboard() {
  const [status, setStatus] = useState("parado");
  const [espIP, setEspIP] = useState<string | null>(null);
  const [espOnline, setEspOnline] = useState(false);

  useEffect(() => {
    const interval = setInterval(async () => {
      try {
        // pegar info geral
        const info = await fetch(`${API_URL}/api/info`).then(r => r.json());
        setEspIP(info.esp_ip);
        setEspOnline(info.online);

        // pegar status da rota
        const rota = await fetch(`${API_URL}/api/status_rota`).then(r => r.json());
        setStatus(rota.estado);

      } catch (e) {
        setEspOnline(false);
      }
    }, 2000);

    return () => clearInterval(interval);
  }, []);

  return (
    <div className="space-y-8">

      <div className="bg-white shadow-lg border-l-8 border-[#815c23] p-8 rounded-xl">
        <h2 className="text-2xl font-bold text-navy mb-4">Status do Robô</h2>

        <div className="text-lg space-y-1 text-black">
          <p><strong>ESP32 IP:</strong> {espIP || "detectando..."}</p>

          <p>
            <strong>Status do ESP:</strong>{" "}
            {espOnline ? (
              <span className="font-semibold text-green-600">online</span>
            ) : (
              <span className="font-semibold text-red-600">offline</span>
            )}
          </p>
        </div>

        <div className="mt-6">
          <span className="px-4 py-2 rounded text-white bg-navy text-lg shadow-lg">
            {status.toUpperCase()}
          </span>
        </div>
      </div>

    </div>
  );
}
