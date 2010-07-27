local server = { }
server[27910] = "aq2.pelitutka.fi:27920"
server[27920] = "aq2.pelitutka.fi:27910"

local bantime = 30

-- simple hack to tell in ClientBegin if this is a new connecting client
local is_connecting = { }
local ip_seen = { }
local ip_ban = { }
function ClientConnect(client)
    local plr = players[client]

    -- check for reconnect loop
    if ip_seen[plr.ip] ~= nil and os.time() - ip_seen[plr.ip] < 10 and (ip_ban[plr.ip] == nil or ip_ban[plr.ip] < os.time()) then
	ip_ban[plr.ip] = os.time() + bantime
    end

    if ip_ban[plr.ip] ~= nil and ip_ban[plr.ip] > os.time() then
	return false, "Reconnect loop detected. Continuing in "..(ip_ban[plr.ip] - os.time()).. " seconds"
    end

    is_connecting[client] = true
    ip_seen[plr.ip] = os.time()
end

function ClientDisconnect(client)
    is_connecting[client] = nil
end

function LevelChanged(level)
    -- reset temp bans and seens on level change
    ip_seen = { }
    ip_ban = { }
end

function ClientBegin(client)
    local plr = players[client]
    local numplrs = 0
    local maxclients = tonumber(gi.cvar("maxclients"))
    local port = tonumber(gi.cvar("port"))

    for i=1,maxclients do
	if players[i].inuse then
	    numplrs = numplrs + 1
	end
    end

    if numplrs == maxclients and is_connecting[client] == true and type(server[port]) == "string" then
	gi.cprintf(client, PRINT_HIGH, "This server is full, forwarding to "..server[port]..".\n")
	stuffcmd(client, "connect "..server[port].."\n")
    end

    is_connecting[client] = nil
end