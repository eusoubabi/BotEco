import { Link, Outlet, useLocation } from "react-router-dom";

export default function App() {
  const location = useLocation();

  function isActive(path: string) {
    return location.pathname === path ? "nav-active" : "nav-item";
  }

  return (
    <div className="flex h-screen bg-[#00112b] relative">
      {/* SIDEBAR */}
      <aside className="w-64 bg-[#815c23] shadow-xl border-r border-[#815c23] p-6 flex flex-col gap-6">

        {/* LOGO / TÍTULO */}
        <h1 className="text-2xl font-bold text-white drop-shadow">
          Robô Entregas
        </h1>

        {/* MENU */}
        <nav className="flex flex-col gap-2 text-lg">

          <Link
            to="/"
            className={`px-3 py-2 rounded transition ${isActive("/")}`}
          >
            Dashboard
          </Link>

          <p className="font-semibold text-white/80 mt-4">Entregas</p>

          <Link
            to="/nova"
            className={`px-3 py-2 rounded transition ${isActive("/nova")}`}
          >
            Nova Entrega
          </Link>

          <Link
            to="/historico"
            className={`px-3 py-2 rounded transition ${isActive("/historico")}`}
          >
            Histórico
          </Link>

          <Link
            to="/relatorios"
            className={`px-3 py-2 rounded transition ${isActive("/relatorios")}`}
          >
            Relatórios
          </Link>

          <Link
            to="/estornos"
            className={`px-3 py-2 rounded transition mt-4 ${isActive("/estornos")}`}
          >
            Estornos
          </Link>
        </nav>
      </aside>

      {/* ÁREA PRINCIPAL */}
      <main className="flex-1 overflow-y-auto p-10 text-white relative">
        <Outlet />

        {/* LOGO NO CANTO INFERIOR DIREITO */}
        <img
          src="/logo.png"
          alt="Logo Robô"
        className="fixed bottom-6 right-0 w-72 opacity-100  select-none pointer-events-none"
        />
      </main>
    </div>
  );
}
