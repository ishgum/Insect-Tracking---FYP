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

% file_name = 'serial_data_test_each1.txt';

%% Hysteresis test, forward back
in_V = ones(200, 2);
in_V = 0.40*in_V;
scale = (0.4:0.1:2.8)';
scale = repmat(scale,15,2);
scale = sort(scale);
% scale(5:5:end) = scale;
% repmat(scale,1,2)
% interp(2:2:end) = chirp;

for I= 1:10
    scale(I:15:end)=0.4;
end
scale = [scale; flipud(scale)];

in_V = scale;
file_name = 'serial_data_test_scale_fb.txt';

%% Hysteresis test, Left right
in_V = ones(200, 2);
in_V = 0.40*in_V;
scale = (0.8:0.1:2.2)';
scale = repmat(scale,15,1);
scale = [scale 1.4*ones(length(scale),1)];
scale = sort(scale);
scale = [scale; scale(:,2), flipud(scale(:,1))];

for I= 1:10
    scale(I:15:end)=0.4;
end

in_V = scale;
file_name = 'serial_data_test_scale_lr.txt';

%% Hysteresis test, Left right, fine
% in_V = ones(200, 2);
% in_V = 0.40*in_V;
% scale = (1.2:0.02:1.6)';
% scale = repmat(scale,15,1);
% scale = [scale 1.4*ones(length(scale),1)];
% scale = sort(scale);
% 
% for I= 1:10
%     scale(I:15:end)=0.4;
% end
% scale = [scale; scale(:,2), flipud(scale(:,1))];
% 
% in_V = scale;
% file_name = 'serial_data_test_scale_lrfine.txt';

%% Pulse averaging test
% in_V = ones(200, 2);
% in_V = 0.40*in_V;
% in_V(10:14,:) = repmat([1 1],5,1);
% % in_V(30:34,:) = repmat([2 1],5,1);
% in_V(50:54,:) = repmat([1 2],5,1);
% % in_V(70:74,:) = repmat([2.5 2.5],5,1);
% in_V(90:94,:) = repmat([1.5 1.5],5,1);
% 
% in_V(11,1) = 0.98;
% in_V(51,1) = 0.4;
% in_V(91,1) = 2.0;
% 
% file_name = 'serial_data_test_scale_ave.txt';

%% Save
quickieConv(in_V, file_name);

%% PLot
figure
plot(1:length(in_V),in_V(:,1),'b', 1:length(in_V), in_V(:,2),'-.r', 'LineWidth', 2);
legend 'Left' 'Right'
ylabel 'V'
xlabel 'Sample'
title 'Pulse train for simulating RSSI over serial'
