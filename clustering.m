%% Small Script to plot and check the statistics of the samples
clc; clear(); close all;

%folder
folder = 'basketball';
featureAmount = 72;

for frame=1:1
    
    %% Loading Data
    %Loading all Samples of all channels
    negSamples = importdata([folder '\frame' num2str(frame) '\negSamples.txt']);
    posSamples = importdata([folder '\frame' num2str(frame)  '\posSamples.txt']);
    negMuSigSq = importdata([folder '\frame' num2str(frame) '\negMuSigSq.txt']);
    posMuSigSq = importdata([folder '\frame' num2str(frame)  '\posMuSigSq.txt']);
    origImg = imread([folder '\groundTruth' num2str(frame)  '.png']);
    sampPositions = importdata([folder '\frame' num2str(frame) '\samplePositions.txt']);
    
    
    %Positive Mean and variance
    meanValuesPositive = posMuSigSq(:,1);
    varValuesPositive = posMuSigSq(:,2);
    meanValuesNegative = negMuSigSq(:,1);
    varValuesNegative = negMuSigSq(:,2);
    
    %Negative mean and variance
    mu_pos = meanValuesPositive;
    sigma_pos = sqrt(varValuesPositive);
    mu_neg = meanValuesNegative;
    sigma_neg = sqrt(varValuesNegative);
    
    %Basketball intervall 17 - 56
    
    %% Clustering
    
    X_0 = zeros(72,10);
    X_1 = zeros(22,10);
    X_box = zeros(40,10);
    X_surr = zeros(32,10);
    chnnl = 1;
    fg1 = figure;
    fg2 = figure;
    for r=1:72:720
    
        surrInv = [r, r+15;r+56, r+71];
        boxInv = [r+16, r+55];
        
        trueObj = [r+26, r+47];
        
        bckgrndFtr = negSamples(r:r+71,1);
        surroundFtr = [posSamples(surrInv(1,1):surrInv(1,2),1)
            posSamples(surrInv(2,1):surrInv(2,2),1)];
        boxFtr = posSamples(boxInv(1):boxInv(2),1);
        trueObjFtr = posSamples(trueObj(1):trueObj(2),1);
        
        X_0(:,chnnl) = bckgrndFtr;
        X_1(:,chnnl) = trueObjFtr;
        X_surr(:,chnnl) = surroundFtr;
        X_box(:,chnnl) = boxFtr; 
        chnnl = chnnl+1;
    end
    
    figure(fg1);
    plot3(X_0(:,1),X_0(:,2),X_0(:,3),'x','LineWidth',2);
    hold on   
    plot3(X_surr(:,1),X_surr(:,2),X_surr(:,3),'x','LineWidth',2);
    plot3(X_box(:,1),X_box(:,2),X_box(:,3),'x','LineWidth',2);
    plot3(X_1(:,1),X_1(:,2),X_1(:,3),'x','LineWidth',2);
    grid;
    hold off
    title('Color Feature Space');
    legend('bckgrnd','surrounding','box','true objct');

%     figure(fg2);
%     plot3(X_0(:,4),X_0(:,5),X_0(:,6),'x','LineWidth',2);
%     hold on   
%     plot3(X_surr(:,4),X_surr(:,5),X_surr(:,6),'x','LineWidth',2);
%     plot3(X_box(:,4),X_box(:,5),X_box(:,6),'x','LineWidth',2);
%     plot3(X_1(:,4),X_1(:,5),X_1(:,6),'x','LineWidth',2);
%     grid;
%     hold off
%     title('Color Feature Space');
%     legend('bckgrnd','surrounding','box','true objct');

    
    
end