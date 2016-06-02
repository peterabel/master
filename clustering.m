%% Small Script to plot and check the statistics of the samples
clc; clear(); close all;

%folder
folder = 'basketball';
featureAmount = 72;

for frame=1:1
    
    %% Loading Data
    %Loading all Samples of all channels
    if isunix == 1
        negSamples = importdata([folder '/frame' num2str(frame) '/negSamples.txt']);
        posSamples = importdata([folder '/frame' num2str(frame)  '/posSamples.txt']);
        negMuSigSq = importdata([folder '/frame' num2str(frame) '/negMuSigSq.txt']);
        posMuSigSq = importdata([folder '/frame' num2str(frame)  '/posMuSigSq.txt']);
        origImg = imread([folder '/groundTruth' num2str(frame)  '.png']);
        sampPositions = importdata([folder '/frame' num2str(frame) '/samplePositions.txt']);
    else
        negSamples = importdata([folder '\frame' num2str(frame) '\negSamples.txt']);
        posSamples = importdata([folder '\frame' num2str(frame)  '\posSamples.txt']);
        negMuSigSq = importdata([folder '\frame' num2str(frame) '\negMuSigSq.txt']);
        posMuSigSq = importdata([folder '\frame' num2str(frame)  '\posMuSigSq.txt']);
        origImg = imread([folder '\groundTruth' num2str(frame)  '.png']);
        sampPositions = importdata([folder '\frame' num2str(frame) '\samplePositions.txt']);        
    end
    
    
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
    X_allftr = zeros(72,10);
    chnnl = 1;
    fg1 = figure;
%     fg2 = figure;
    for r=1:72:720
    
        surrInv = [r, r+15;r+56, r+71];
        boxInv = [r+16, r+55];
        
        trueObj = [r+26, r+47];
        
        bckgrndFtr = negSamples(r:r+71,1);
        surroundFtr = [posSamples(surrInv(1,1):surrInv(1,2),1)
            posSamples(surrInv(2,1):surrInv(2,2),1)];
        boxFtr = posSamples(boxInv(1):boxInv(2),1);
        trueObjFtr = posSamples(trueObj(1):trueObj(2),1);
        allftr = posSamples(r:r+71,1);
        
        X_0(:,chnnl) = bckgrndFtr;
        X_1(:,chnnl) = trueObjFtr;
        X_surr(:,chnnl) = surroundFtr;
        X_box(:,chnnl) = boxFtr;
        X_allftr(:,chnnl) = allftr;
        chnnl = chnnl+1;
    end
    
    X = X_box(:,1:2);
    
    figure(fg1);
    plot(X_0(:,1),X_0(:,2),'x','LineWidth',2);
    hold on
    plot(X_surr(:,1),X_surr(:,2),'x','LineWidth',2);
    plot(X_box(:,1),X_box(:,2),'x','LineWidth',2);
    plot(X_1(:,1),X_1(:,2),'x','LineWidth',2);
    grid;
    hold off
    title('Color Feature Space');
    legend('bckgrnd','surrounding','box','true objct');
  
    
%% GMM Clustering
%     k = 2;
%     Sigma = {'diagonal','full'};
%     nSigma = numel(Sigma);
%     SharedCovariance = {true,false};
%     SCtext = {'true','false'};
%     nSC = numel(SharedCovariance);
%     d = 500;
%     x1 = linspace(min(X(:,1)) - 2,max(X(:,1)) + 2,d);
%     x2 = linspace(min(X(:,2)) - 2,max(X(:,2)) + 2,d);
%     [x1grid,x2grid] = meshgrid(x1,x2);
%     X0 = [x1grid(:) x2grid(:)];
%     threshold = sqrt(chi2inv(0.99,2));
%     options = statset('MaxIter',1000); % Increase number of EM iterations
%     
%     figure;
%     c = 1;
%     for i = 1:nSigma;
%         for j = 1:nSC;
%             gmfit = fitgmdist(X,k,'CovarianceType',Sigma{i},...
%                 'SharedCovariance',SharedCovariance{j},'Options',options);
%             clusterX = cluster(gmfit,X);
%             mahalDist = mahal(gmfit,X0);
%             subplot(2,2,c);
%             h1 = gscatter(X(:,1),X(:,2),clusterX);
%             hold on;
%             for m = 1:k;
%                 idx = mahalDist(:,m)<=threshold;
%                 Color = h1(m).Color*0.75 + -0.5*(h1(m).Color - 1);
%                 h2 = plot(X0(idx,1),X0(idx,2),'.','Color',Color,'MarkerSize',1);
%                 uistack(h2,'bottom');
%             end
%             plot(gmfit.mu(:,1),gmfit.mu(:,2),'kx','LineWidth',2,'MarkerSize',10)
%             title(sprintf('Sigma is %s, SharedCovariance = %s',...
%                 Sigma{i},SCtext{j}),'FontSize',8)
%             hold off
%             c = c + 1;
%         end
%     end

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