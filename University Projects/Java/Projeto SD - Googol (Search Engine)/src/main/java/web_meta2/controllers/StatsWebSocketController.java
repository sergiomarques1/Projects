package web_meta2.controllers;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.messaging.handler.annotation.MessageMapping;
import org.springframework.messaging.handler.annotation.SendTo;
import org.springframework.stereotype.Controller;
import web_meta2.models.StatsService;

import java.util.HashMap;
import java.util.Map;

/**
 * Controlador WebSocket específico para estatísticas
 * Permite obter estatísticas sob demanda, além das atualizações periódicas
 */
@Controller
public class StatsWebSocketController {

    @Autowired
    private StatsService statsService;

    /**
     * Endpoint para solicitar uma atualização imediata das estatísticas
     * O cliente pode chamar este endpoint quando entrar na página de estatísticas
     */
    @MessageMapping("/request-stats-update")
    @SendTo("/topic/stats-update")
    public Map<String, Object> requestStatsUpdate() {
        System.out.println("Recebida solicitação de atualização de estatísticas via WebSocket");

        // Aciona a coleta de estatísticas imediatamente
        try {
            statsService.coletarEstatisticas();

            // Resposta de confirmação
            Map<String, Object> response = new HashMap<>();
            response.put("status", "ok");
            response.put("message", "Atualização de estatísticas solicitada");
            response.put("timestamp", System.currentTimeMillis());

            return response;
        } catch (Exception e) {
            System.err.println("Erro ao solicitar atualização de estatísticas: " + e.getMessage());
            e.printStackTrace();

            // Resposta de erro
            Map<String, Object> response = new HashMap<>();
            response.put("status", "error");
            response.put("message", "Erro ao solicitar estatísticas: " + e.getMessage());
            response.put("timestamp", System.currentTimeMillis());

            return response;
        }
    }
}