clear
clc
close all
%% Real tests
% load('yagiTestData\yagitest3.mat');
% in_V = test3_FullRot30(:,2:3);
% quickieConv(in_V, 'serial_data_test_test3_FR30.txt');
% test3_Distance

%% Artificial tests
%% One pulse in each state:
len = 100;
% volts
in_V = ones(len, 2);
in_V = 0.40*in_V;
in_V(10:14,:) = repmat([1 1],5,1);
in_V(30:34,:) = repmat([2 1],5,1);
in_V(50:54,:) = repmat([1 2],5,1);
in_V(70:74,:) = repmat([2.5 2.5],5,1);
in_V(90:94,:) = repmat([1.5 1.5],5,1);

%% Save
quickieConv(in_V, 'serial_data_test_each1.txt');

%% PLot
plot(1:len,in_V(:,1),'b', 1:len, in_V(:,2),'-.r', 'LineWidth', 2);
legend 'Left' 'Right'
ylabel 'V'
xlabel 'Sample'
title 'Pulse train for simulating RSSI over serial'
