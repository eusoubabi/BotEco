import { useEffect, useState } from "react";

const API_URL = import.meta.env.VITE_API_URL || "http://127.0.0.1:5000";

export default function Dashboard() {
  const [status, setStatus] = useState("parado");
  const [itemAtual, setItemAtual] = useState<string | null>(null);
  const [destinoAtual, setDestinoAtual] = useState<string | null>(null);
  const [botaoRetornarAtivo, setBotaoRetornarAtivo] = useState(false);

  useEffect(() => {
    const interval = setInterval(async () => {
      try {
        const r = await fetch(`${API_URL}/api/status_rota`);
        const data = await r.json();

        setStatus(data.estado);

        if (data.estado === "chegou") {
          setBotaoRetornarAtivo(true);
        }
      } catch {}
    }, 1000);

    return () => clearInterval(interval);
  }, []);

  (window as any).iniciarEntrega = (item: string, destino: string) => {
    setItemAtual(item);
    setDestinoAtual(destino);
    setBotaoRetornarAtivo(false);
  };

  async function retornar() {
    if (!destinoAtual) return;

    const rota = destinoAtual === "A" ? "a_volta" : "b_volta";
    await fetch(`${API_URL}/api/rota/${rota}`, { method: "POST" });
    setBotaoRetornarAtivo(false);
  }

  async function parar() {
    await fetch(`${API_URL}/api/parar`, { method: "POST" });
  }

  return (
    <div className="space-y-8">

      <div className="bg-white shadow-lg border-l-8 border-[#815c23] p-8 rounded-xl">
        <h2 className="text-2xl font-bold text-navy mb-4">Status do Robô</h2>

        {itemAtual && (
          <div className="space-y-1 text-lg">
            <p><strong>Item:</strong> {itemAtual}</p>
            <p><strong>Destino:</strong> Setor {destinoAtual}</p>
          </div>
        )}

        {/* STATUS COM DESTAQUE */}
        <div className="mt-6">
          <span className="px-4 py-2 rounded text-white bg-navy text-lg shadow-lg">
            {status.toUpperCase()}
          </span>
        </div>

        {status === "chegou" && (
          <p className="text-gold font-semibold text-lg mt-4">
            O robô chegou ao destino. Pronto para retornar.
          </p>
        )}

        <div className="mt-8 flex gap-4">
          <button
            onClick={parar}
            className="px-6 py-3 bg-red-600 text-white rounded-lg shadow hover:bg-red-700 transition"
          >
            Parar
          </button>

          <button
            disabled={!botaoRetornarAtivo}
            onClick={retornar}
            className={`px-6 py-3 rounded-lg shadow transition ${
              botaoRetornarAtivo
                ? "bg-cyan text-navy hover:bg-cyan/80"
                : "bg-gray-300 text-gray-500 cursor-not-allowed"
            }`}
          >
            Retornar
          </button>
        </div>
      </div>

    </div>
  );
}
