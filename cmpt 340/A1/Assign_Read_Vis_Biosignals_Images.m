%% CMPT340 Assignment - Reading & Visualizing Bio-signals
% ============================ Goal =========================:
%   In this activity, you will load and visualize different data  
%   types acquired from different modalities. 
%
% ===================== Download Data   ==============================
%
% First, download and unzip the BioData from Canvas: 
%
%
% The data folder contains 5 files:
%
%       (1) emg_myopathy.txt (EMG)
%
%       (2) bMode.mat (B-mode Ultrasound)
%
%       (3) vol4d.mat (fMRI)
%
%       (4) XYStripes20x20Noise_9_L_3.mat (tensor field)
%       
%       (5) clathrin_ROI_cmpt340.csv (SMLM data)
%                 
%% ============================ Task 1 ===============================
%% (1) EMG Data 
% Data Description: EMG is short for electromyogram, which measures electrical activity in muscles
% Requirement: 
%      Load the data from emg_myopathy.txt and extract the two columns of values: 
%%             i) First column -- time
%%             ii) Second column -- the amplitude of an EMG signal. 

close all; clear; clc;
% Load data into MATLAB:
% Write your own code here:
emg_data = readmatrix('emg_myopathy.txt');
 
% Q1: What are the dimensions of the loaded variable (eg. 5 x 3 x 10)? 
% Write your own code here: 
dim_emg_data = size(emg_data);

fprintf('Variable dimansions: %g X %g\n',dim_emg_data(1,1),dim_emg_data(1,2)); % display your answer

% How many EMG amplitude values does the signal contain?
% Write your own code here:

fprintf('Number of amplitudes: %g\n', dim_emg_data(1,1)); % display your answer

% Plot the amplitude vs time using the values in the two columns. 
% Properly label both axes. Add a suitable title to your figure.
figure(1)
plot(emg_data(:,1),emg_data(:,2));
title("EMG time data");
xlabel("Time");
ylabel("Amplitutde")

% There is a period of time with a clearly reduced EMG amplitude. 
% Examine the previous plot, use the "Zoom In" icon on the toolbar 
% to close up on this region of the plot. 
% Visually (roughly) identify the approximate starting and ending 
% time for this period.
% Display these values :
% Write your own code here
fprintf('Start : %g  End: %g\n', 16.7, 16.9); % update the display 

% What are the time and amplitude values of the 100th sample of the data? 
% Write your own code here

% update the display
fprintf('Time_100th = %g\n',emg_data(100,1));
fprintf('Amp_100th = %g\n',emg_data(100,2));

% Plot the EMG signal from the 100th sample (in (1d)) up until the 1100th
% sample. Give a title to your figure. 
figure(2)
plot(emg_data(100:1100,1),emg_data(100:1100,2));
title("Sampled EMG signal");
xlabel("Time");
ylabel("Amplitude");


%% ============================ Task 2 ===============================
%% (2) B-mode Ultrasound sequence 
% Data Description: This data represent an B-mode ultrasound video sequence (B stands for Brightness)
% Requirement: Load the data in bMode.mat into MATLAB, explore the video frames and create a GIF
%  
close all; clear; clc;
% Load data into MATLAB:
% Write your own code here:
load("bMode.mat");
dim_bMode = size(bMode);

% What are the dimensions of the loaded variable (eg. 5 x 3 x 10)? 
% You should output the variable dim_bMode as a vector of size 1x3.. 
% Write your own code here
% dim_ulta = size(ultra_data);

fprintf('bMode dimensions: %g X %g X %g\n',dim_bMode(1,1),dim_bMode(1,2),dim_bMode(1,3)); % display your answers

% How many frames (2D images) do we have in the video? 
% Write your own code here

fprintf('Number of frames: %g\n',dim_bMode(1,3)); % display your answer

% What is the size (rows and columns) of each frame?
% Write your own code here
fprintf('Nb_rows: %g\n',dim_bMode(1,1)');
fprintf('Nb_cols: %g\n',dim_bMode(1,2)');

% Extract and display the 9-th frame of the sequence in a figure. 
% Apply the command that ensures that pixels are not stretched 
% (i.e. maintain aspect ratio of the image). Apply the command 
% that uses a grey colour-map to display the image.
% Write your own code here
figure(1)
bMode_sample = bMode(:,:,9);
imagesc(bMode_sample);
axis image;
colormap gray;
title("Sampled bMode")


% What is the intensity value at row 30 and column 40, at frame 20?
% Write your own code here 
fprintf('Intensity_(30,40,20): %g', bMode(30,40,20)); % update the display to show your answer

% Extract a cropped rectangular region of frame 15. 
% The cropped part should extend from row 10 to row 30, and from column 45 
% to column 60 of the image.  Display this cropped region 
% (also called region of interest or ROI).
% Write your own code here
figure(2)
bMode_sample2 = bMode(10:30,45:60,15);
imagesc(bMode_sample2);
axis image;
title("Sampled bMode 2")
 % update the colormap and figure title
%% 

% Create an animated GIF that shows the ultrasound sequence
% (like a video clip). 
% Save this GIF image using the name US_seq.gif . 
% You can make use of the MATLAB code used during the lectures
% Write your own code here 
filename = fullfile("US_seq.gif");
for a=1:30
    img = bMode(:,:,a);
    img = mat2gray(img);
  
    [A,map] = gray2ind(img,256);
    if a == 1
        imwrite(A,map,filename,'gif','LoopCount',Inf,'DelayTime',0.25);
    else
         imwrite(A,map,filename,'gif','WriteMode','append','DelayTime',0.25);
    end
end


%% ============================ Task 3 ===============================
%% (3) Functional MRI
% Data Description: This is a 3D+time f-MRI data (functional magnetic resonance image). 
% fMRI captures the changes in oxygenation with time (4th dimension) at every (x,y,z) location of a volume.
% Requirement: Load the data in vol4d.mat into MATLAB, explore how oxygenation level changes with time
% 
close all; clear; clc;
% Load data into MATLAB:
% Write your own code here:
load("vol4d.mat");

% What are the dimensions of the loaded variable (eg. 5 x 3 x 10)? 
% Write your own code here:
dim_vold4d = size(vol4d);

fprintf('Size fMRI: %g %g %g %g\n',dim_vold4d(1,1),dim_vold4d(1,2),dim_vold4d(1,3),dim_vold4d(1,4)); % display your answer

% Noting that the first 3 dimensions represent the spatial x, y, and z 
% dimensions, identify how many rows, columns, slices, and time steps 
% does this fMRI data have?
% Write your own code here:

% update the display:
fprintf('Size fMRI rows: %g\n',dim_vold4d(1,1)); % display # of rows in each slice
fprintf('Size fMRI cols: %g\n',dim_vold4d(1,2)); % display # of columns in each slice
fprintf('Size fMRI slices: %g\n',dim_vold4d(1,3)'); % display # of slices
fprintf('Size fMRI time: %g\n',dim_vold4d(1,4)'); % display time

% Plot a curve that shows how the oxygenation level changes 
% with time at voxel location (x=32, y=34 , z=10).
% Define axis and figure title. 
figure(1)
vol4d_sample = squeeze(vol4d(32,34,10,:));
plot(1:20,vol4d_sample);
xlabel("Time");
ylabel("Oxygenation level");
title("Sample 1 oxygenation level")


% Extract and display the slice in the X-Y plane, at slice 
% location z=15, of the 10th time sample.
figure(2)
vol4d_sample2 = (vol4d(:,:,15,10));
imagesc(vol4d_sample2);
xlabel("Time");
ylabel("Oxygenation level");
title("Sample 2 oxygenation level")


% Extract and visualize the slice in the X-Z plane, 
% at slice location y=10, of the 15th time sample.
figure(3)
vol4d_sample3 = squeeze(vol4d(:,10,:,15));
imagesc(vol4d_sample3);
xlabel("Time");
ylabel("Oxygenation level");
title("Sample 3 oxygenation level")


% Create an animated GIF that shows how the slice in the previous question
% changes with time (over the whole time steps). 
% Save this GIF image using the name fMRI.gif. 
% You can make use of the MATLAB code used during the lectures. 

% Write your own code here:
filename = fullfile("fMRI.gif");
for a=1:20
    img = squeeze(vol4d(:,10,:,a));
    img = mat2gray(img);  
     [A,map] = gray2ind(img,256);
    if a == 1
        imwrite(A,map,filename,'gif','LoopCount',Inf,'DelayTime',0.25);
    else
         imwrite(A,map,filename,'gif','WriteMode','append','DelayTime',0.25);
    end
end


%% ============================ Task 4 ===============================
%% (5) Diffusion Tensor MRI 
% Data Description: This is a single 2D slice of a Diffusion Tensor MRI data, 
% which is a 2D image with a 3x3 matrix at every pixel.
% Requirement: Load the data XYStripes20x20Noise_9_L_3.mat into MATLAB, calculate the eigenvalues.

close all; clear; clc;
% Load data into MATLAB:
% Write your own code here:
load("XYStripes20x20Noise_9_L_3.mat");
dim_tensor = size(TensorIM);

% What is the dimensions of the loaded variable (eg. 5 x 3 x 10)? 

% Write your own code here:

fprintf('Size fMRI: %g X %g X %g X %g \n', dim_tensor(1,1), dim_tensor(1,2),dim_tensor(1,3),dim_tensor(1,4)); %display your answer 

% How many rows and columns of pixels does this 2D image have?

% Write your own code here: 

fprintf(['Nb rows: %g', ', Nb cols: %g\n'],dim_tensor(1,1), dim_tensor(1,2)); % update to display the correct values

% Extract and display (as an array of numbers) the 3x3 
% matrix at pixel location: (row 10, column 15). Use variable name *arr3x3*

% your code here, update the display to print your variables
arr3X3 = squeeze(TensorIM(10,15,:,:));

disp('Array at x=10, y=15 : ');
disp(arr3X3);

% Use the "eig" function in MATLAB to calculate and display the 3 eigen
% vectors and the corresponding 3 eigen values of the 3x3 matrix arr3x3;
% Use the variable name *arr3x3_eig*.

% your code here, add a display at the end of your code to print out your
% answers
[arr3X3_eig, arr3X3_val] = eig(arr3X3);

disp('Eigenvectors : '); 
disp(arr3X3_eig);
fprintf('Eigenvalues : %g, %g, %g\n',arr3X3_val(1,1),arr3X3_val(2,2),arr3X3_val(3,3)); 

% Create and display a 2D image *DT_2D* with a scalar number at every pixel. 
% The scalar number should be the entry (row=1, column=2) of the 
% 3x3 matrix at every pixel.

% Write your own code here:
figure(1)
DT_2D = zeros(20, 20);
for row = 1: 20
    for col = 1:20
        DT_2D(row,col) = squeeze(TensorIM(row,col,1,2));
    end
end
imagesc(DT_2D);
axis image;
title("DT-2D image");

% Create and display a 2D image with a scalar number at every pixel. 
% The scalar number should be the largest eigenvalue of the 3x3 matrix at 
% each pixel. 
% The challenge is to do this WITHOUT using loops (without for or while).

% Using loops
% DT_2D_all = zeros(20, 20);
% for row = 1: 20
%     for col = 1:20
%         eig_val = eig(squeeze(TensorIM(row,col,:,:)));
%         max_eig_val = max(eig_val);
% 
%         DT_2D_all(row,col) = max_eig_val;
%     end
% end
% imagesc(DT_2D_all);
% axis image;
% title("DT-2D image of max eigen values of all voxels");
% figure(2)

% Not using loops
figure(2)
D = num2cell(TensorIM,3:4);
D_resized = cellfun(@(matrix) reshape(matrix, 3, 3), D, 'UniformOutput', false);
D_eigenval = cellfun(@eig,D_resized, 'UniformOutput',false);
D_eigenval_max = cellfun(@max,D_eigenval, 'UniformOutput',false);
D_mat = cell2mat(D_eigenval_max);
imagesc(D_mat);


%% ============================ Task 5 ===============================
%% (6) SMLM -- Single-Molecule Localization Microscopy
% Data Description: 3D point cloud data illustrating clathrin protein locations. 
% The 3 channels in the 2nd column represents 3D coordinates.

close all; clear; clc;
% Load data into MATLAB:
% Write your own code here:
clathrin_data = readmatrix("clathrin_ROI_cmpt340.csv");

% What is the dimensions of the loaded variable (eg. 5 x 3 x 10)? 
% your code here
dim_clathrin = size(clathrin_data);

fprintf('Size SMLM: %g X %g\n', dim_clathrin(1,1), dim_clathrin(1,2)); %display your answer 

% Select 1001 points from the imported data. You can clip from the middle, e.g., use format such as rawData(ind-xxx:ind:xxx)
% Write your code here

clathrin_sample = clathrin_data(round(end/2)-500: round(end/2)+500,:);

% Plot the raw data
figure(1)
scatter3(clathrin_sample(:,1),clathrin_sample(:,2),clathrin_sample(:,3),20);
colormap('jet');
Ax = gca;
Ax.Color = 'k';
Ax.XAxis.Color = 'red';
Ax.YAxis.Color = 'red';
Ax.ZAxis.Color = 'red';
colorbar;
xlabel('X nm');
ylabel('Y nm');
zlabel('Z nm');
grid on;
rotate3d;
 % you can use proper 'position' property to make the figure look nice
% Write your code here

% Use colormap jet, black background
% Label X, Y, Z dimensions properly, display Unit = 'nm'
% rotate camera to visualize your data

% Construct graph (this method only works for small data) 
% this is the naive way to construct the graph using the distance matrix (for small data)
PT = 800; % set proximity threshold -- proximity threshold means how you decide to clip the data according to the distances among neighboring points
% Calculate pairwise distance among all the points
distDat = pdist(clathrin_sample);
distMat = squareform(distDat);
distMat_th = distMat .* (distMat <= PT);
% Use adjacency matrix representation
% Threshold the adjacency distance matrix
% Create a graph from the distance matrix, save it as variable g
g = graph(distMat_th,array2table(clathrin_sample));

% What if you selected more than 1001 points from the data 
% When the size of the data is large, creating a graph from 
% the (thresholded) distance matrix can produce out-of-memory errors. 
% An alternative is to call the graph by supplying the source, the target, 
% and the weights of the edges in the graph, i.e.,
%      g = graph(source, target, weight);
% 
% Write the code (which involves a loop over the number of nodes) 
% that can create source, target and weight from d=pdist(rawDat);
% Write your code here
x = 5000;
clathrin_sample_2 = clathrin_data(1:x,:);
source = [];
target = [];
weight = [];
for i=1:x
    for j = (i+1):x
        d_ij = pdist2(clathrin_sample_2(i,:),clathrin_sample_2(j,:));
        if(d_ij < PT)
            source = [source, i];
            target = [target, j];
            weight = [weight, d_ij];

        end
    
    end
end

g2 = graph(source,target,weight);


% plot the constructed graph preserving the spatial info
figure(2)
p = plot(g2, 'XData',clathrin_sample_2(:,1),'YData',clathrin_sample_2(:,2),'ZData',clathrin_sample_2(:,3),'NodeLabel',{} ); % Fill in the code here
axis equal;
xlabel = "X nm";
ylabel = "Y nm"; 
zlabel = "Z nm";
p.Marker = 'o';
p.MarkerSize = 2;
p.NodeColor = 'red';
p.EdgeColor = [1,1,1];
Ax = gca;
Ax.Color = 'k';


% set marker to 'o', use a marker size=2, red color and white edge color
% set X, Y, Z axis labels
% Write your code here
% find the node degree, save it into variable deg
% Write your code here
deg = degree(g2);
% plot the points colored by their degree
figure(3)
scatter3(clathrin_sample_2(:,1),clathrin_sample_2(:,2),clathrin_sample_2(:,3),20,deg,'filled');
xlabel = "X nm"; ylabel = "Y nm"; zlabel = "Z nm";
colormap('jet');
colorbar;
Ax = gca;
Ax.Color = 'k';
Ax.XAxis.Color = "red";
Ax.YAxis.Color = "red";
Ax.ZAxis.Color = "red";

% Write your code here
% use colormap jet, black background, set X, Y, Z axis labels using unit='nm'
% Write your code here
%%%%%%%%%%%%%%
figure(4)
% For the original graph (created using the adjacency matrix) change graph visualization layout to 'force' 
% Write your code here
p1 = plot(g,'Layout',"force");
% Fill in the code here
p1.EdgeColor = [1 0 0];
p1.Marker = 'o';
p1.MarkerSize = 3;
p1.NodeColor = 'b';
xlabel = "X nm"; ylabel = "Y nm"; zlabel = "Z nm";
axis equal;






