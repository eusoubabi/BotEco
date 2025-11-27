const API_URL = import.meta.env.VITE_API_URL || "http://127.0.0.1:5000";

import { useState } from "react";

export default function NewDelivery() {
  const [item, setItem] = useState("");
  const [destino, setDestino] = useState("A");
  const [loading, setLoading] = useState(false);

  async function enviar() {
    if (!item) return alert("Informe o nome do item!");
    setLoading(true);

    const rota = destino === "A" ? "a_ida" : "b_ida";

    try {
      const r = await fetch(`${API_URL}/api/rota/${rota}`, { method: "POST" });

      if (!r.ok) {
        alert("Falha ao comunicar com o backend.");
        setLoading(false);
        return;
      }

      (window as any).iniciarEntrega(item, destino);

      alert(`Entrega enviada ao Setor ${destino}!`);
      setItem("");
    } catch {
      alert("Não foi possível se comunicar com o robô.");
    }

    setLoading(false);
  }

  return (
    <div className="bg-white shadow-lg border-l-8 border-[#815c23] p-8 rounded-xl max-w-xl">
      <h2 className="text-2xl font-bold text-navy mb-6">Nova Entrega</h2>

      <label className="font-semibold text-black">Item:</label>
      <input
        className="w-full border p-3 rounded mb-6 text-black"
        value={item}
        onChange={(e) => setItem(e.target.value)}
      />

      <label className="font-semibold text-black">Destino:</label>
      <select
        className="w-full border p-3 rounded mb-6 text-black"
        value={destino}
        onChange={(e) => setDestino(e.target.value)}
      >
        <option className="text-black" value="A">Setor A</option>
        <option className="text-black" value="B">Setor B</option>
      </select>

      <button
        onClick={enviar}
        disabled={loading}
        className={`px-6 py-3 rounded-lg shadow text-white transition
          ${loading ? "bg-gray-400" : "bg-navy hover:bg-navy/90"}`}
      >
        {loading ? "Enviando..." : "Enviar"}
      </button>
    </div>
  );
}
