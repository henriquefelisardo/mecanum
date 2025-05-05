const express = require('express');
const WebSocket = require('ws');
const path = require('path');
const rateLimit = require('express-rate-limit');

const app = express();
const port = 3000;

// Configuração do rate limiting
const limiter = rateLimit({
    windowMs: 1000,
    max: 10
});

app.use(express.static(path.join(__dirname, 'public')));
app.use('/api/commands', limiter);

const server = app.listen(port, '0.0.0.0', () => {
    console.log(`Servidor rodando em http://localhost:${port}`);
});

const wss = new WebSocket.Server({ server });

// Estado compartilhado
let activeCommands = new Set();
let commandsQueue = [];

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
            
            // Armazena comandos para o ESP32
            commandsQueue.push(data.command);
            
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

// Endpoint para o ESP32
app.get('/api/commands', (req, res) => {
    res.json({
        commands: commandsQueue.splice(0, commandsQueue.length) // Esvazia a fila ao ler
    });
});