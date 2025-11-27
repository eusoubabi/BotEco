import React from "react";
import ReactDOM from "react-dom/client";
import { BrowserRouter, Routes, Route } from "react-router-dom";

import App from "./App";
import Dashboard from "./Dashboard";
import NewDelivery from "./NewDelivery";
import DeliveryHistory from "./DeliveryHistory";
import DeliveryReports from "./DeliveryReports";
import Reversals from "./Reversals";

import "./index.css";
import "./globals.css";

ReactDOM.createRoot(document.getElementById("root")!).render(
  <React.StrictMode>
    <BrowserRouter>
      <Routes>
        <Route path="/" element={<App />}>
          <Route index element={<Dashboard />} />
          <Route path="nova" element={<NewDelivery />} />
          <Route path="historico" element={<DeliveryHistory />} />
          <Route path="relatorios" element={<DeliveryReports />} />
          <Route path="estornos" element={<Reversals />} />
        </Route>
      </Routes>
    </BrowserRouter>
  </React.StrictMode>
);
