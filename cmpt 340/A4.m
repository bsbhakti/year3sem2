%% PART 1

x1 = [7,8,2,8,6,1,3,5,8,8,2,8,8,4,7,2,4,8,7,8];
mean_x1 = sum(x1)/length(x1);

mean_devia_x1 = 1/length(x1)* sum(abs(x1-mean_x1));

skewness_x1 = 1/length(x1) * (sum((x1 - mean_x1) .^3) / (1/length(x1) * sum ((x1- mean_x1) .^2)) .^(3/2));

kurtosis_x1 = (1/length(x1) * (sum((x1 - mean_x1) .^4) / (1/length(x1) * sum ((x1- mean_x1) .^2)) .^(2))) -3;


flatness_x1 = exp((1/length(x1) * sum(log(x1)) )) / ((1/length(x1)) * sum(x1));

bin_edges = 0.5:1:max(x1)+0.5;
histogram_x1 = histogram(x1,bin_edges);
xlim([0.5, max(bin_edges)]);
title('Histogram of Signal');
xlabel('Amplitude');
ylabel('Frequency')

corr_1 = corr_cal(x1,x1);

x1_delayed_minus_one = [8,2,8,6,1,3,5,8,8,2,8,8,4,7,2,4,8,7,8,0];
corr_minus_one = corr_cal(x1,x1_delayed_minus_one);

x1_delayed_one = [0,7,8,2,8,6,1,3,5,8,8,2,8,8,4,7,2,4,8,7,8];
x1_len_21 = [7,8,2,8,6,1,3,5,8,8,2,8,8,4,7,2,4,8,7,8,0];
corr_one = corr_cal(x1_len_21,x1_delayed_one);



x1_delayed_two = [0,0,7,8,2,8,6,1,3,5,8,8,2,8,8,4,7,2,4,8,7,8];
x1_len_22 = [7,8,2,8,6,1,3,5,8,8,2,8,8,4,7,2,4,8,7,8,0,0];
corr_two = corr_cal(x1_len_22,x1_delayed_two);


x1_delayed_three = [0,0,0,7,8,2,8,6,1,3,5,8,8,2,8,8,4,7,2,4,8,7,8];
x1_len_23 = [7,8,2,8,6,1,3,5,8,8,2,8,8,4,7,2,4,8,7,8,0,0,0];
corr_three = corr_cal(x1_len_23,x1_delayed_three);



%% PART TWO
x1 = [7,8,2,8,6,1,3,5,8,8,2,8,8,4,7,2,4,8,7,8];
x2 = [0.66,0.04,0.85,0.93,0.68,0.76,0.74,0.39,0.66,0.17,0.71,0.03,0.28,0.05,0.10,0.82,0.69,0.32,0.95,0.03];
scatter(x1,x2);
title("Scatter between x1 and x2");
xlabel("x1");
ylabel("x2");

corr_x1_x2 = corr_cal(x1,x2);

x1_r = tiedrank(x1);
x2_r = tiedrank(x2);

corr_x1_r_x2_r = corr_cal(x1_r, x2_r);

x2_delayed_minus_one = [0.04,0.85,0.93,0.68,0.76,0.74,0.39,0.66,0.17,0.71,0.03,0.28,0.05,0.10,0.82,0.69,0.32,0.95,0.03,0];

corr_x1_x2_minus_one = corr_cal(x1,x2_delayed_minus_one);

x2_delayed_three = [0,0,0,0.66,0.04,0.85,0.93,0.68,0.76,0.74,0.39,0.66,0.17,0.71,0.03,0.28,0.05,0.10,0.82,0.69,0.32,0.95,0.03];

x1_len_23 = [7,8,2,8,6,1,3,5,8,8,2,8,8,4,7,2,4,8,7,8,0,0,0];
corr_x1_x2_delayed_3 = corr_cal(x1_len_23,x2_delayed_three);

corr_x1_x2_delayed_0 = corr_cal(x1,x2);

bin_edges_x1 = 1:9; % Bin edges for x1: [1, 2, 3, ..., 9]
bin_edges_x2 = 0.1:0.1:1; % Bin edges for x2: [0.1, 0.2, ..., 1]

% Initialize the 2D histogram matrix
histogram_2d = zeros(length(bin_edges_x1)-1, length(bin_edges_x2)-1);

% Loop through each pair of bin edges
for i = 1:length(bin_edges_x1)-1
    for j = 1:length(bin_edges_x2)-1
        % Count occurrences of values in the current bin
        count = sum(x1 >= bin_edges_x1(i) & x1 < bin_edges_x1(i+1) & ...
                    x2 >= bin_edges_x2(j) & x2 < bin_edges_x2(j+1));
        % Assign the count to the corresponding entry in the 2D histogram
        histogram_2d(i, j) = count;
    end
end

% Display the 2D histogram matrix
disp("2D Histogram Matrix:");
disp(histogram_2d);

%% PART 3
heart_2d_x = [1,0.5,1,4,5.5,5,1];
heart_2d_y = [1,3,5,5,3,1,1];
perimeter = 0;

for i=1:length(heart_2d_x)-1
    perimeter = perimeter + distance_cal(heart_2d_x(i),heart_2d_y(i), ...
        heart_2d_x(i+1),heart_2d_y(i+1));

end

diameter = 0;
x1_pt= 0;
y1_pt = 0;
x2_pt= 0;
y2_pt = 0;

for i=1:length(heart_2d_x)
    for j = 1:length(heart_2d_x)
        dist = distance_cal(heart_2d_x(i),heart_2d_y(i),heart_2d_x(j),heart_2d_y(j));
        if(dist > diameter)
            diameter = dist;
            x1_pt = heart_2d_x(i);
            y1_pt = heart_2d_y(i);
            x2_pt = heart_2d_x(j);
            y2_pt = heart_2d_y(j);
        end
    end
end

cordB = distance_cal(4,5,1,1);
eccentricity = diameter/cordB;
area = 17;
compactness = (perimeter)^2/area;

centroid = [sum(heart_2d_x)/length(heart_2d_x),sum(heart_2d_y)/length(heart_2d_y)];

CG = distance_cal(centroid(1,1),5.5,centroid(1,2),3);

DG = distance_cal(centroid(1,1),4,centroid(1,2),5);
EG = distance_cal(centroid(1,1),1,centroid(1,2),5);
FG = distance_cal(centroid(1,1),0.5,centroid(1,2),3);
AG = distance_cal(centroid(1,1),1,centroid(1,2),1);
BG = distance_cal(centroid(1,1),5,centroid(1,2),1);

slope_GC = slope_cal(centroid(1,1),5.5,centroid(1,2),3);


angle_GC_GD = angle_cal(slope_GC,slope_cal(centroid(1,1),4,centroid(1,2),5)); 52.42
angle_GC_GE = angle_cal(slope_GC,slope_cal(centroid(1,1),1,centroid(1,2),5)); 118.93
angle_GC_GF = angle_cal(slope_GC,slope_cal(centroid(1,1),0.5,centroid(1,2),3)); 166.57
angle_GC_GA = angle_cal(slope_GC,slope_cal(centroid(1,1),1,centroid(1,2),1));138.08
angle_GC_GB = angle_cal(slope_GC,slope_cal(centroid(1,1),5,centroid(1,2),1));139.2



%% PART 4

slice_z1 = [
    0 0 0 ;
    0 0 0;
    0 0 0 ;
   
];

slice_z2 = [
    1 0 0 ;
    0 0 0 ;
    0 0 1;
 
];

slice_z3 = [
    0 0 0 ;
    0 0 0;
    0 0 0 ;
];

SHAPE = cat(3, slice_z1, slice_z2, slice_z3);

% Display the created 3D array

% isosurface(SHAPE,0.5);

xbar = m_cal(1,0,0,SHAPE)/m_cal(0,0,0,SHAPE);
ybar = m_cal(0,1,0,SHAPE)/m_cal(0,0,0,SHAPE);
zbar = m_cal(0,0,1,SHAPE)/m_cal(0,0,0,SHAPE);
m100 = m_cal(1,0,0,SHAPE);

mu_001 = mu_cal(0,0,1,xbar,ybar,zbar,SHAPE)
mu_000 = mu_cal(0,0,0,xbar,ybar,zbar,SHAPE)
mu_200 = mu_cal(2,0,0,xbar,ybar,zbar,SHAPE)
%% 

mu_000 = mu_cal(0,0,0,xbar,ybar,zbar,SHAPE);
mu_200 = mu_cal(2,0,0,xbar,ybar,zbar,SHAPE);
mu_020 = mu_cal(0,2,0,xbar,ybar,zbar,SHAPE);
mu_002 = mu_cal(0,0,2,xbar,ybar,zbar,SHAPE);

J1 = mu_200 + mu_020 + mu_002;
centroid_3d_ans = centroid_cal_3d(SHAPE);

%% 
function corr_val = corr_cal(x1, y1)
n = length(x1);
sum_x1 = sum(x1);
sum_y1= sum(y1);
sum_x1_y1 = sum (x1 .* y1);
sum_x1_sq = sum(x1 .* x1);
sum_y1_sq = sum(y1 .* y1);
nu =  (n * sum_x1_y1) - (sum_x1 .* sum_y1);
de = sqrt((n * sum_x1_sq - ((sum_x1)^2)) * ( n * sum_y1_sq - ((sum_y1) ^2)));
corr_val =   nu/de;
end


function dist_val = distance_cal(x1,y1,x2,y2)
    dist_val = sqrt((x2-x1)^2+ (y2-y1)^2);
end


function angle = angle_cal(slope_line1, slope_line2)
%     angle_rad = atan(abs((slope_line2 - slope_line1) / (1 + slope_line1 * slope_line2)));

  angle_rad1 = atan(abs((slope_line2 - slope_line1) / (1 + slope_line1 * slope_line2)));
angle_rad2 = pi - angle_rad1;  % Obtuse angle in radians

% Choose the larger angle (either acute or obtuse)
angle_rad = max(angle_rad1, angle_rad2);
angle_rad_sm = min(angle_rad1, angle_rad2);


% Convert angle to degrees
    angle = rad2deg(angle_rad);
    angle_sm = rad2deg(angle_rad_sm)
    disp(angle_rad_sm)
    disp(angle)


end

function slopes = slope_cal(x1,x2,y1,y2)
    slopes = (y2 - y1) / (x2 - x1);
end


function mu_val = mu_cal(p,q,r,xbar,ybar,zbar,SHAPE)
mu_val =0;
for x =1:3
    for y=1:3
        for z=1:3
            mu_val = mu_val +((x-xbar)^p *(y-ybar)^q * (z-zbar)^r * SHAPE(x,y,z));

        end
    end
end

end

function m_val = m_cal(p,q,r,SHAPE)
m_val = 0;
for x =1:3
    for y=1:3
        for z=1:3
            m_val = m_val +(x^p *y^q*z^r* SHAPE(x,y,z));

        end
    end
end
end


function centroid_3d = centroid_cal_3d(SHAPE)
centroid_3d = [0,0,0];
for x =1:4
    for y=1:5
        for z=1:3
            centroid_3d(1,1) = centroid_3d(1,1)+  ( x* SHAPE(x,y,z));
            centroid_3d(1,2) = centroid_3d(1,2)+  ( y* SHAPE(x,y,z));
            centroid_3d(1,3) = centroid_3d(1,3)+  ( z* SHAPE(x,y,z));
        end
    end
end
centroid_3d = centroid_3d/12;
end


