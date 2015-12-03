%% Thing2
clc
clear
close all;
format shortg

%% Consts
Fixed_proc = 0.01;          % pesstimate
Cost_resolution = 5e-8;     % seconds per pixel
Uav_lag = 1 ;               % seconds
Bug_speed = 5        ;      % ms-1
Min_px = 5         ;        % one dimension of bug in image, in px
Bug_size = 0.02    ;        % one dim of bug in reality, m
Min_dst = 3;                % m
Max_dst = 10;
FOS = 2;                    % fos 2 = image is twice what bug can move
                            % fos 4 = bug can only move halfway to edge
Max_res = 2048;%400;%2048;

% Can't do 3D plots!

%% Plot lag for different FOS



% lag = Cost_resolution+px_width^2;
syms lag vel_bug fos_width cost_px px_min_bug bug_size px
eqn = 4*cost_px*(vel_bug*fos_width*px_min_bug*lag/bug_size)^2 - lag == 0
solve(eqn,lag);

subs(eqn,[cost_px, fos_width, vel_bug, px_min_bug, bug_size],...
    [5e-8, 2, 5, 20, 0.02]);


vel_bug_v = linspace(1, 10, 10);
lag = zeros(1,length(vel_bug));
for i = 1:length(vel_bug_v)
    temp = solve(subs(eqn,[cost_px, fos_width, vel_bug, px_min_bug, bug_size],...
    [5e-8, 2, vel_bug_v(i), 20, 0.02]));
    if length(temp) >1
        laga(i) = eval(temp(2));
    else
        laga(i) = eval(temp);
    end
end
plot(laga)
% px = Max_res
% width = 4.096
% width/(px)
% px_bug = Bug_size*px/width
% 
% distance = linspace(Min_dst, Max_dst, 10);
% px = Min_px*width/Bug_size
