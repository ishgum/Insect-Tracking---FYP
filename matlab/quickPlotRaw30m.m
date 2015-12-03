% crappy plot raw 30m test data
clear
clc
close all
% t = 1.5;
% 2*142.5420
load('yagiTestData\yagitest3.mat');
left = (test3_FullRot30(:,2));
right = (test3_FullRot30(:,3));
time = (test3_FullRot30(:,1));
time = time/1040;
n = 150;
left = [0.5*ones(n,1);left(1:end-n)];
plot(time,left,time,right)
ylim([0, 2])
xlim([0, 10])
legend('Left Yagi', 'Right Yagi')
ylabel 'RSSI (V)'
xlabel 'Time (s)'