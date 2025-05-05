const express = require('express');
const WebSocket = require('ws');
const path = require('path');

const app = express();
const port = 3000;

app.use(express.static(path.join(__dirname, 'public')));

const server = app.listen(port, '0.0.0.0', () => {
    console.log(`Servidor rodando em http://localhost:${port}`);
});

const wss = new WebSocket.Server({ server });

// Armazena os comandos ativos de todos os clientes
let activeCommands = new Set();

wss.on('connection', (ws) => {
    console.log('Novo cliente conectado');
    
    // Envia os comandos ativos para o novo cliente
    ws.send(JSON.stringify({
        type: 'initial',
        commands: Array.from(activeCommands)
    }));

    ws.on('message', (message) => {
        try {
            const data = JSON.parse(message);
            
            // Atualiza o conjunto de comandos ativos
            if (data.action === 'press') {
                activeCommands.add(data.command);
            } else {
                activeCommands.delete(data.command);
            }
            
            // Broadcast para todos os clientes
            wss.clients.forEach(client => {
                if (client.readyState === WebSocket.OPEN) {
                    client.send(JSON.stringify({
                        type: 'update',
                        command: data.command,
                        action: data.action
                    }));
                }
            });
        } catch (error) {
            console.error('Erro ao processar mensagem:', error);
        }
    });

    ws.on('close', () => {
        console.log('Cliente desconectado');
    });
});