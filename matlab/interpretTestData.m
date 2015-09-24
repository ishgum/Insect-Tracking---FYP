%% Displaying Test data in a sweet way
%{

Random commands
% find(isnan(test2_LtoR(1:28500,1)),1)

%}
clear
clc
close all

%% Import data:
% First time import & save to mat
% really slow importing from large excel files,
% so use pre-imported .mat files instead, if possible
% the following is for first-time importing

% For yagitest2Final.xlsx:
% test2_LtoR = xlsread('yagitest2Final.xlsx', 'Sheet2', 'L1:N54126');
% test2_FullRot = xlsread('yagitest2Final.xlsx', 'Sheet3', 'L1:N101839');
% test2_Distance = xlsread('yagitest2Final.xlsx', 'Sheet5', 'L1:N276649');
% save('yagiTestData\yagitest2Final.mat')

% For yagitest3.xlsx:
% test3_FullRot10 = xlsread('yagitest3.xlsx', 'Sheet1', 'E3:G99678');
% test3_FullRot30 = xlsread('yagitest3.xlsx', 'Sheet2', 'F3:H175540');
% test3_Distance = xlsread('yagitest3.xlsx', 'Sheet3', 'D3:F162171');
% save('yagiTestData\yagitest3.mat')
% 
% For succeeding times just load .mats:
load('yagiTestData\yagitest2Final.mat');
load('yagiTestData\yagitest3.mat');

% test2_LtoR(:,2) = removeNoise(test2_LtoR(:,2));
% test2_LtoR(:,3) = removeNoise(test2_LtoR(:,3));
% % remove the 
% plot(test2_LtoR(:,1),test2_LtoR(:,2),test2_LtoR(:,1),test2_LtoR(:,3))
% 
% % left = removeNoisePoints(test2_LtoR(:,1:2));
% % right = removeNoisePoints([test2_LtoR(:,1) test2_LtoR(:,3)]);
% % hold on
% % % plot(left(:,1),left(:,2), right(:,1), right(:,2),'r')
% % plot(left(:,1),left(:,2))
% % % plot(test2_LtoR(:,1),test2_LtoR(:,2));
% % % remove the     
% % 
% % % fit poly, a trig func would be better . . 
% % polyco = polyfit(left(:,1),left(:,2),4)
% % plot(left(:,1),polyval(polyco, left(:,1)))
% 
% 
%% First test
% %{
% with V1 antennas
% Annoying data to work with
% %}
% 
%% another test?
% left = removeNoisePoints(test2_FullRot(:,1:2));
% right = removeNoisePoints([test2_FullRot(:,1) test2_FullRot(:,3)]);
% hold on
% % plot(left(:,1),left(:,2), right(:,1), right(:,2),'r')
% plot(left(:,1)/1000,left(:,2))
% figure
% scatter(left(:,1)/1000,left(:,2))
% 
% knownAngles =  [15   -60
%                 19  -60
%                 23  -45
%                 43  -10
%                 48  10
%                 62  45
%                 73  90
%                 88  135
%                 99  180
%                 111 -135
%                 123 -90
%                 139 -45];
% % transform to polar map
% % rough conversion:
% % travel -60 to -45 so 375 deg
% leftAngles = [1:(375/length(left)):375]';
% leftAngles = leftAngles-60;
% leftAngles = leftAngles + (leftAngles>180).*leftAngles*-2;
% rightAngles = [1:(375/length(right)):375]';
% 
%% Second test
%{
7/08/2015
with rebuilt antennas (V2)

%}
% ~10m circle (centre circle)
left = removeNoisePoints(test3_FullRot10(:,1:2));
%trim beginning & end:
left = left(6:end-17,:);
right = removeNoisePoints([test3_FullRot10(:,1) test3_FullRot10(:,3)]);
right = right(6:end-17,:);
% hold on
% scatter(left(:,1),left(:,2));
% scatter(right(:,1), right(:,2),'r');
% polar plot
% knownAngles = [
 % transform to polar map
% rough conversion:
% travel 90 to 90 so 360 deg
leftAngles = [1:(360/length(left)):360]';
leftAngles = (135-leftAngles);
leftAngles = deg2rad(leftAngles);
% figure
% hold on
% grid on
% polar(leftAngles, left(:,2))
% hold on
% view([90 -90])

rightAngles = [1:(360/length(right)):360]';
rightAngles = (45-rightAngles);
rightAngles = deg2rad(rightAngles);
% figure
% 
% polar(rightAngles, right(:,2),'r')
% view([90 -90])

%% 30m circle (guess with measure)
left = removeNoisePoints(test3_FullRot30(:,1:2));
% trim beginning & end:
left = left(6:end-5,:);
right = removeNoisePoints([test3_FullRot30(:,1) test3_FullRot30(:,3)]);
right = right(6:end-5,:);
% hold on
% scatter(left(:,1),left(:,2));
% scatter(right(:,1), right(:,2),'r');

% leftAngles = [1:(360/length(left)):360]';
leftAngles = linspace(0,360,149)';
leftAngles = (45-leftAngles);
leftAngles = deg2rad(leftAngles);
% figure
% hold on
% grid on
polar(leftAngles, left(:,2))
title('Left antenna')
% hold on
view([90 -90])

% rightAngles = [1:(360/length(right)):360]';
rightAngles = linspace(0,360,149)';
rightAngles = (315-rightAngles);
rightAngles = deg2rad(rightAngles);
figure

polar(rightAngles, right(:,2),'r')
legend('Right Antenna');%, 'Right Antenna')

% view([90 -90])

%% super cool plot for 30m:
% setAngle sets the angle between the two overlaid antenna patterns
setAngle = 60;
leftAngles = (leftAngles-deg2rad(setAngle/2));
rightAngles = (rightAngles+deg2rad(setAngle/2));

% Modify for poster display:
left = left(:,2);
left = left - 0.49;
right = right(:,2);
right = right- 0.49;

left = maf(left, 20);
right = maf(right, 20);
left = [left; left(1)];
right = [right; right(1)];
leftAngles = [leftAngles; leftAngles(1)];
rightAngles = [rightAngles; rightAngles(1)];



figure
axis equal
polar(leftAngles, left)%:,2))
hold on

polar(rightAngles, right,'r')%(:,2),'r')
view([90 -90])
legend('Left Antenna', 'Right Antenna');
title(sprintf('Antennae at %2.0f degrees apart\n\n',setAngle))
% Actual for yt3fr30m
actualAngles = [0       10
                -45		29
                -90		64
                -135	88
                180		121
                135		155
                90		182
                45		205
                0       233];
% actualAnglesInterp = 

% function fig = getParentFigure(fig)
% % if the object is a figure or figure descendent, return the
% % figure. Otherwise return [].
% while ~isempty(fig) & ~strcmp('figure', get(fig,'type'))
%   fig = get(fig,'parent');
% end

% 	
% 
% % distance (guess with measure)
% figure
distance = [10	36
            20	48
            30	60
            40	68
            50	78
            60	88
            70	98
            80	110
            90	122
            100   135
            90	150
            80    159
            70    170
            60	180
            50	190
            40	198
            30    207
            20    218
            10    226
            0     233];
% find(time<143&time>142)
time = test3_Distance(:,1)/1000;
% time(97500:end) = 2*142.5420-time(97500:end);
left = test3_Distance(:,2);
right = test3_Distance(:,3);
figure
plot(time, left,'-')
figure
plot(time, right,'-')
% figure
% plot(time(90000:120000), right(90000:120000),'-')

far_right = right(90000:110000)-0.48;
figure
plot(1:length(far_right), far_right)
v_noise = 0.2
far_right_sig = (far_right>v_noise).* far_right;
far_right_noise = (far_right<=v_noise).* far_right;
figure
plot(1:length(far_right), far_right_sig)
figure
plot(1:length(far_right), far_right_noise)

% powers
PWR_s = far_right_sig'*far_right_sig
PWR_n = far_right_noise'*far_right_noise
SNR = 10*log10(PWR_s/PWR_n)
% plot(time, left,'-', time, right,'-')
% left = removeNoisePoints([time, left]);
% right = removeNoisePoints([time, right]);
% figure
% plot(left(:,1), left(:,2),'-', right(:,1), right(:,2),'-')

%% Third test
%{
Future
with V2 antennas, repeated to investigate prior test results

%}
% 30m circle

% distance

% UAV motor noise test