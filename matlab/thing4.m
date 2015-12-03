%% Thing4
clc
clear
close all
format shortg

%% Consts
Width = linspace(0,10,9)
lag = 0.04;
v = 5;
FOS = 2;

D = 2*FOS*(Width/15+lag)*v
% plot(distance, focal)