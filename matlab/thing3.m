% Thing3
clc
clear
close all;
format shortg

% Consts
Width = 4.8;
Min_dst = 4;                % m
Max_dst = 20;
Sensor = 0.011264;

distance = linspace(Min_dst, Max_dst, 10);
focal = distance*Sensor/Width;
plot(distance, focal)