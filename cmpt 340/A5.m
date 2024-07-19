%% QUESTION 1
diagnosis_mat = load('diagnosis.mat');
GGO_val_mat = load('GGO_value.mat');
diagnosis_mat_positive = diagnosis_mat.diagnosis ==1;
diagnosis_mat_positive_values = GGO_val_mat.GGO_values(diagnosis_mat_positive);
diagnosis_mat_negative = diagnosis_mat.diagnosis ==0;
diagnosis_mat_negative_values = GGO_val_mat.GGO_values(diagnosis_mat_negative);

mean_negative_values = mean(diagnosis_mat_negative_values);
mean_positive_values = mean(diagnosis_mat_positive_values);
std_positive_values = std(diagnosis_mat_positive_values);
std_negative_values = std(diagnosis_mat_negative_values);

x= 0:1:120;
positive_graph = normpdf(x, mean_positive_values,std_positive_values);
negative_graph = normpdf(x, mean_negative_values,std_negative_values);
h1 = plot(x,positive_graph,'r-','LineWidth',3);
hold on;
h2 = plot(x,negative_graph,'g-','LineWidth',3);
legend([h1,h2], "Class +ve", "Class -ve");
xlabel("GGO value");
ylabel("Probability");
title("Probablity distributions of positive and negative decision");
grid on;

%% QUESTION 2

figure;
grid on;hold on;
xlabel('FPR');
ylabel('TPR');
title('ROC Curve using Gaussian model');
for T=20:5:80
    FPR = 1-normcdf(T,mean_negative_values,std_negative_values);
    TPR = 1-normcdf(T,mean_positive_values,std_positive_values);
    plot(FPR , TPR ,'-ko', 'MarkerSize',10,'MarkerFaceColor',[0 1 1],'LineWidth',2)
    text(FPR+.01 , TPR+0.01, num2str(T),'Color',[0.5 0.5 0.5])
    
end


figure;
grid on;hold on;
xlabel('FPR');
ylabel('TPR');
title('ROC Curve using err model');

for T=20:5:80
    z1 = (T - mean_positive_values)/(std_positive_values);
    z2 = (T - mean_negative_values)/(std_negative_values);

    FPR = 1-  (0.5 * (1+ (erf(z2/sqrt(2)))));
    TPR = 1-  (0.5 *(1+ (erf(z1/sqrt(2)))));
    plot(FPR , TPR ,'-ko', 'MarkerSize',10,'MarkerFaceColor',[1 0 1],'LineWidth',2)
    text(FPR+.01 , TPR+0.01, num2str(T),'Color',[0.5 0.5 0.5])
    
end


%% Question 3
V = [70,60,30,80,40,20,50,90,85,45,75,65,55,15,35,45,45,65,65,75];
C = [1, 1, 0, 1, 0, 0, 0, 1, 1, 0, 1, 1, 1, 0, 0, 0, 0, 1, 1, 0];

positive = (V(C==1))';
negative = (V(C==0))';
positive_sorted = sort(positive);
negative_sorted = sort(negative);

bin_edges = 0:10:100;

figure
h1 = histogram(positive_sorted,bin_edges, 'FaceColor',[1 0.4 0.4],'FaceAlpha',0.6);
hold on

h2 = histogram(negative_sorted,bin_edges, 'FaceColor',[0.4 1 0.4],'FaceAlpha',0.6);
legend([h1,h2], "Class +ve", "Class -ve");
ylabel('count')

nPos=sum(C==1);
nNeg=sum(C==0);

V_sorted = sort(V);
possibe_solutions = [];
possibe_FNR = [];
possibe_FPR = [];
for T=50:5:80
    model_prediction_N = V <= T;
    model_prediction_P = V >= T;

    model_prediction=zeros(size(V));
    model_prediction(model_prediction_N)=0;
    model_prediction(model_prediction_P)=1;

    TP = (model_prediction==1) & (C==1);
    FN = (model_prediction==0) & (C==1);
    FP = (model_prediction==1) & (C==0);
    TN = (model_prediction==0) & (C==0);
    TNR = sum(TN) * 100/nNeg;
    FNR = sum(FN) * 100/nNeg;TPR = sum(TP) * 100/nPos;FPR = sum(FP) * 100/nPos;

    if(FNR <= 10 && FPR <=40)
        tuple = {FNR, FPR, T};
        disp(tuple)

        possibe_solutions = [possibe_solutions, T];
        possibe_FNR = [possibe_FNR, FNR];
        possibe_FPR = [possibe_FPR, FPR];

%choose 55 lowest both
    end
    
end
T = 55;
 model_prediction_N = V < T;
    model_prediction_P = V >= T;

    model_prediction=zeros(size(V));
    model_prediction(model_prediction_N)=0;
    model_prediction(model_prediction_P)=1;

    TP = (model_prediction==1) & (C==1);
    FN = (model_prediction==0) & (C==1);
    FP = (model_prediction==1) & (C==0);
    TN = (model_prediction==0) & (C==0);
    TNR = sum(TN) * 100/nNeg;
    FNR = sum(FN) * 100/nNeg;TPR = sum(TP) * 100/nPos;FPR = sum(FP) * 100/nPos;

misdiagnosed = ((FPR + FNR) /100) *20; % FPR = 1-TPR  = 20%  = 4
sick_as_healthy = FNR;
healthy_as_sick = FPR;

