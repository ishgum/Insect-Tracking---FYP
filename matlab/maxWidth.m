% max width
% based on min mag
M2 = 20*(5.5e-6)^2/(160^-6) %m?
M = sqrt(M2);
hi = 0.011264; %m
ho = hi/M

A = 160*2048^2/20
Am = A/1e6
sqrt(Am)