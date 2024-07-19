g_samples = [3,2,11,12,5,6,13,9,8,0,1,3,11,16,4,7,9,4,6,5,3,2,4,3,15,9,8,4,17,18,10,12,13,8,9,2,14,16,5,10,6,9,11,3,12,14,13];
h = histogram(g_samples,'BinWidth',1);
%% 
f=1/0.3;           %frequency of the impulse in Hz
fs=f*10;            % sample frequency is 10 times higher
t=0.6:1/fs:14.4;    % time vector
y=zeros(size(t));
y(1:fs/f:end)=1;
xticks(0.6:0.3:14.4);   % Set the location of the ticks
plot(t,y);

%% 
plot(g_samples);
f = fft(g_samples);
% plot(f)
%%
g_samples = [3, 2, 11, 12, 5, 6, 13, 9, 8, 0, 1, 3, 11, 16, 5, 7, 9, 4, 6, 5, 3, 2, 4, 3, 15, 9, 8, 4, 17, 18, 10, 12, 13, 8, 9, 2, 14, 16, 5, 10, 6, 9, 11, 3, 12, 14, 13];

% Define the moving average filter
h = ones(1, 3) / 3;

% Perform convolution
convolution_result = conv(g_samples, h, 'same');

% Display the result
disp(squeeze(convolution_result));
