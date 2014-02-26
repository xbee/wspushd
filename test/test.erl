#!/usr/bin/env escript
%%! +P 999999

-module(test).
-mode(compile).

-export([main/1]).

get_timestamp() ->
  {Mega, Sec, Micro} = os:timestamp(),
  (Mega*1000000 + Sec)*1000 + round(Micro/1000).
    

start([WsUrl, ID]) ->
    process_flag(trap_exit, true),
    WS = rfc6455_client:new(WsUrl, self()),
    % io:format("[*] Connecting ~p ~p ~n", [WsUrl, ID]),
    Data = ""++integer_to_list(get_timestamp()),
    % io:format("[~p]  ~p ~n", [ID, Data]),
    WS ! {send, Data},
    loop(WS, ID).

loop(WS, ID) ->
    receive
        Q = {'EXIT',_,_} ->
            io:format("Child process died together with parent (~p)\n",[Q]);
        {rfc6455, open, WS, Opts} ->
            % io:format("[~p] http ~p~n", [ID, Opts]),
            loop(WS, ID);
        {rfc6455, recv, WS, Payload} ->
            % io:format("[~p] Recv ~p~n", [ID, Payload]),
            timer:sleep(6000),
            Data = ""++integer_to_list(get_timestamp()),
            WS ! {send, Data},
            loop(WS, ID);
        {rfc6455, close, WS, R} ->
            io:format("[~p] Closed ~p~n", [ID, R]),
            ok
    end.

main([WsUrl]) ->
    start_test(WsUrl, 100000),
    % timer:sleep(2000),
    % start_test(WsUrl, 2000),
    % timer:sleep(2000),
    % start_test(WsUrl, 2000),
    % timer:sleep(2000),
    % start_test(WsUrl, 2000),
    start([WsUrl, 1]).
    % spawn(?MODULE, wstest, [[WsUrl]]).


start_test(Url, N) ->  
    % folsom:start(),
    % folsom_metrics:new_counter(pktcnt),
    % spawn N processes
    io:format(" Start to test~n"),
    F = fun(K) -> 
          % io:format(" spawn wstest~n"),
          spawn(fun() -> start([Url, K]) end),
          timer:sleep(1)
          % spawn(testclient, start, [[Url]])
        end,
    lists:map(F, lists:seq(2,N)).  % F must have one argument

