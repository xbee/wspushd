
-module(testclient).


-export([start/1]).

get_timestamp() ->
  {Mega, Sec, Micro} = os:timestamp(),
  (Mega*1000000 + Sec)*1000 + round(Micro/1000).
    

start([WsUrl]) ->
    process_flag(trap_exit, true),
    WS = rfc6455_client:new(WsUrl, self()),
    io:format("[*] Connecting ~p ~n", [WsUrl]),
    Data = "ping "++integer_to_list(get_timestamp()),
    io:format("[*] ~p ~n", [Data]),
    WS ! {send, Data},
    loop(WS).

loop(WS) ->
    receive
        Q = {'EXIT',_,_} ->
            io:format("Child process died together with parent (~p)\n",[Q]);
        {rfc6455, open, WS, Opts} ->
            io:format("[.] http ~p~n", [Opts]),
            loop(WS);
        {rfc6455, recv, WS, Payload} ->
            io:format("[.] Recv ~p~n", [Payload]),
            timer:sleep(6000),
            Data = "ping "++integer_to_list(get_timestamp()),
            WS ! {send, Data},
            loop(WS);
        {rfc6455, close, WS, R} ->
            io:format("[*] Closed ~p~n", [R]),
            ok
    end.

