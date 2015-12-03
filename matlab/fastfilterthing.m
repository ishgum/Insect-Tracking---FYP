% Fast filter stuff
clc
clear
close all

%% Define desired freq characteristics

%% Make the shit dude
ts = 0.0015;

n_maf = 100;
maf = 1/n_maf*ones(1,n_maf);
[h1,w1] = freqz(maf);
grid on
plot(w1/(2*ts),20*log10(h1),'k')
% legend('Ideal', 'Both')
xlabel 'Frequency (Hz)', ylabel 'Magnitude (dB)'
% title('100 Element MAF Filter Frequency Reponse');

% figure
% freqz(maf)
%% Find the normalised freq
% fs = 1.5;
% f_c_lower = 50-20;  % stop below
% f_c_upper = % stop above
%% Make the dude
f = [0 0.01 0.05  0.1 0.3  1]; % normalised freq pairs, 1 = fs/2
a = [0   1    1     0  0  0]; %desired amplitude at freqs
% 
f = [ 0 0.05 0.1 1]
a = [1 1 0 0];
N = 80;

filt = firpm(N, f, a);
% freqz(filt)

%% Data
load('yagiTestData\yagitest3.mat');
left = test3_FullRot10(:,2);
time = test3_FullRot10(:,1);
% figure, plot(time, left)
l = left(1:5000);
t = time(1:5000);
% figure, plot(t, l)

% load('yagiTestData\yagitest2Final.mat');
dist = test3_Distance(:,3);
time_dist = test3_Distance(:,1);
% figure, plot(time, left)
% l = left(1:5000);
% t = time(1:5000);
% figure, plot(t, l)

load('pulse_110dbm.mat')
%p110
%%
% l_filtered = conv(l,(filt));
% figure, plot((l_filtered));

%% compare with removeNoisePoints
[dist_p, ind] = removeNoisePointsWithIndex([time_dist dist]);
% figure, plot(dist_p(:,2))

dist_filtered = conv(dist,(filt));
figure, plot(dist_filtered);
[peaks_d_filt, p_locs] = findpeaks(dist_filtered, 'MinPeakDistance', 500,...
    'MinPeakHeight', 0.6);
p_locs = p_locs(1:end-1);

current_method = zeros(length(dist),1);
fir_method = zeros(length(dist),1);
current_method(ind-10) = dist(ind-10);
fir_method(p_locs-35) = dist(p_locs-35);
% fir_method = fir_method(2:end);

% figure, plot(time_dist, dist ,time_dist, fir_method,'o');
figure, plot(time_dist, current_method, 'x', time_dist, fir_method,'o')