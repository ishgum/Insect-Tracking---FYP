%% Investigate noise floor
%{
Following testing that indicated that there was substantial interference
caused by placement on the UAV, It was decided that this needs to be
formally tested to determine whether the system is workable in it's current
state.

Firstly, to avoid some bias, a guesstimate of the usable level of noise is
 required:
Assuming:
The most sensitive measurement is too far, which relies on being able to
measure the difference between the noise floor and the pusle at 1.0V
If we presume to get 10 samples per pulse then we can simply reject those
pulses which occur in less time.
********************* ???????????????
Also, 0.1V is used as the L & R difference. The effect of noise on this is
not ascertained.
********************* ???????????????
However,
The major problem is that, when there is significant noise, the average
value of the noise floor will be raised.

1. If the average value of the noise floor exceeds 0.8V then it is supposed
to be too noisey.
2. The pulse_threshold is currently 0.5V. This means that the pulse at 1V
must exceed the noise floor by 0.5V to be detected.
Decreasing this to 0.2V does seem reasonable though.

It has been seen that 0.5V is a typical noise level on Ilam fields, without
the UAV.
%}
clear
clc
close all
%% Load
% can use xlsread on tab delimited import
% junk returns NaN
% data = xlsread('UAVtest.xlsx');
% data = xlsread('UAVtestnoDLnoUAVVpowerOutsideRadioUAVpowermotoron.xlsx');
% save('UAV_test_outside_HAK_motor_on.mat')
temp = load('yagiTestData\UAV_test_outside_HAK_motor_on.mat');
data = temp.data;

% Ignore extra cols:
time = data(:,1);
time = time - time(1);
data_2col = data(:,2:3);

% Remove junk rows:
i = 1;
for I = 1:length(data_2col)
    if (~isnan(data_2col(I,1)) && ~isnan(data_2col(I,2)) &&...
            (data_2col(I,1)<5 && data_2col(I,2)<5))
        data_clean_2_col(i,1:2) = data_2col(I,1:2);
        i = i + 1;
    end
end
len_signal = length(data_clean_2_col);
t = 1:len_signal;
%% Find floor level using MAF size 10, 50, 100, and Len
maf10 = 0.1*ones(1,10);
noise_floor_10(:,1) = conv(data_clean_2_col(:,1), maf10);
noise_floor_10(:,2) = conv(data_clean_2_col(:,2), maf10);

maf50 = (1/50)*ones(1,50);
noise_floor_50(:,1) = conv(data_clean_2_col(:,1), maf50);
noise_floor_50(:,2) = conv(data_clean_2_col(:,2), maf50);

maf100 = (1/100)*ones(1,100);
noise_floor_100(:,1) = conv(data_clean_2_col(:,1), maf100);
noise_floor_100(:,2) = conv(data_clean_2_col(:,2), maf100);

mafLen(1,1) = mean(data_clean_2_col(:,1));
mafLen(1,2) = mean(data_clean_2_col(:,2));

%% Plot results
% limit so climb & fall of MAF doesn't show
t = t(100:end);
t = 5:26/(len_signal-100):31;
length(t)
length(noise_floor_100(100:len_signal,1))
length(mafLen(2)*ones(len_signal-99,1))
% plot(t,data_clean_2_col)
hold on
% plot(t,noise_floor_10(1:len_signal,1),'b',t,noise_floor_10(1:len_signal,2),'m')
% plot(t,noise_floor_50(1:len_signal,1),'c',t,noise_floor_50(1:len_signal,2),'m')
plot(t,noise_floor_100(100:len_signal,1),'k',t,noise_floor_100(100:len_signal,2),'r')
plot(t,mafLen(1)*ones(len_signal-99,1),'k--',t, mafLen(2)*ones(len_signal-99,1),'--r')

% Typical noise on fields
% plot(t,0.49*ones(len_signal,1),'k--')
legend('Left 100 MAF', 'Right 100 MAF','Left Average', 'Right Average')
% legend('Left 10 MAF', 'Right 10 MAF','Left 100 MAF', 'Right 100 MAF','Left Average', 'Right Average')
ylabel 'RSSI (V)'
title 'Noise level, RSSI samples without pulse transmitter. On UAV with motors on, near NZi3'
xlabel 'Time (s)'