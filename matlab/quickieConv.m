function [ ] = quickieConv(in_V, filename)
%% quickie convert
% Go from volts to some characters, and saves as ASCI text file.
% Accepts 0.4 to 2.8V
%   10 reserved for EOL
%  no checks for range
%% Settings
start = 15;
stop = 255;
range = stop-start;

start_V = 0.4;
stop_V = 2.8;
range_V = stop_V-start_V;

del_V = range_V/range; % smallest representable voltage step
%% Main
% len = 20;
% len = max(size(in_V))
% % volts
% in_V = ones(len, 2);
% in_V = 0.40*in_V;
% in_V(9,:) = [2.5 2.5];
% in_V(10,:) = [2.5 2.5];

% map to ints, start to stop

in = round((in_V-start_V)/del_V + start);

if (sum(sum(in < 0)) > 0 || sum(sum(in > 255)) > 0)
    disp('!!!!!!!!!!!!!!!          OOBs        !!!!!!!!!!!!!!!!!!!');
end
    
% conv to char
out = char(in);

% add leading EOL
out = [char(10), char(10); out];
% check
% double(out)

% save to file
dlmwrite(['dataForSerialTesting\' filename],out,''); % uses LF = 10.

% text_out = fopen('testData2.txt','w');
% for I = 1:len
%     fprintf(text_out, );
end