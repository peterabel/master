%% Small Script to plot and check the statistics of the samples
clc; clear(); close all hidden;
% Adding Toolbox
addpath(genpath('/home/peter/Dokumente/KIT/Masterarbeit/master/matlab/icf toolbox/toolbox'))

% Initial settings
folder = 'birds1';

dim = 3; %Sum Kernel Dimension
frameNum = 5;

flag.trainClassifier = 0;
flag.testClassifier = 0;


flag.plotGt = 1;
flag.plotMask = 0;
flag.plotChns = 0;
flag.plotLocalSum = 0;
flag.plotFtr = 0;
flag.plotHist = 0;
flag.plotResults = 1;

errorRateAda = ones(frameNum,1);
% errorRateRobu = ones(5,1);


for frame=1:frameNum
    %% Load Images etc
    if isunix == 1
        currFrame = imread([folder '/' sprintf('%8.8d',frame) '.jpg']);
        groundTruth = importfile([folder '/groundtruth.txt']);
    else
        currFrame = imread([folder '\00000001.jpg']);
        groundTruth = importfile([folder '\groundtruth.txt']);
    end
    
    minX = min(groundTruth(frame,1:2:end));
    minY = min(groundTruth(frame,2:2:end));
    maxX = max(groundTruth(frame,1:2:end));
    maxY = max(groundTruth(frame,2:2:end));
    
    width = maxX-minX;
    height = maxY-minY;
    
    if width >= height
        sampBoxImgHeight = width;
        sampBoxImgWidth = width;
        sampBoxImgX = minX;
        sampBoxImgY = minY - 0.5 * abs(sampBoxImgHeight-height);
    end
    if width < height
        sampBoxImgHeight = height;
        sampBoxImgWidth = height;
        sampBoxImgX = minX - 0.5 * abs(sampBoxImgWidth-width);
        sampBoxImgY = minY;
    end
    
    %% Generating Channel Features
    chns = chnsCompute(currFrame);
    
    I1=localSum(chns.data{1,1}(:,:,1),dim,'block','sum');
    I2=localSum(chns.data{1,1}(:,:,2),dim,'block','sum');
    I3=localSum(chns.data{1,1}(:,:,3),dim,'block','sum');
    I4=localSum(chns.data{1,2}(:,:,1),dim,'block','sum');
    I5=localSum(chns.data{1,3}(:,:,1),dim,'block','sum');
    I6=localSum(chns.data{1,3}(:,:,2),dim,'block','sum');
    I7=localSum(chns.data{1,3}(:,:,3),dim,'block','sum');
    I8=localSum(chns.data{1,3}(:,:,4),dim,'block','sum');
    I9=localSum(chns.data{1,3}(:,:,5),dim,'block','sum');
    I10=localSum(chns.data{1,3}(:,:,6),dim,'block','sum');
    
    Is = {I1,I2,I3,I4,I5,I6,I7,I8,I9,I10};
    
    %% Getting positive and negative feature samples
    
    scaleX = (size(currFrame,1)*dim)/size(chns.data{1,1},1);
    scaleY = (size(currFrame,2)*dim)/size(chns.data{1,1},2);
    
    sampBoxChnX = sampBoxImgX/scaleX;
    sampBoxChnY = sampBoxImgY/scaleY;
    sampBoxChnWidth = sampBoxImgWidth/scaleX;
    sampBoxChnHeight = sampBoxImgHeight/scaleY;
    
    v = [groundTruth(frame,1:2); groundTruth(frame,3:4); groundTruth(frame,5:6); groundTruth(frame,7:8)];
    mask = poly2mask(v(:,1),v(:,2),size(currFrame,1),size(currFrame,2));
    maskBox = mask(round(sampBoxImgY):round(sampBoxImgY)+floor(sampBoxImgHeight),...
            round(sampBoxImgX):round(sampBoxImgX)+floor(sampBoxImgWidth));
%     maskDown = maskBox(1:scaleX:end,1:scaleY:end);
    maskDown = imresize(maskBox,[sampBoxChnHeight,sampBoxChnWidth]);
    
    samps = cell(1,10);
    positives = cell(1,10);
    negatives = cell(1,10);
    for m=1:10
        % All Values in the Box
        samps{m} = Is{m}(round(sampBoxChnY):round(sampBoxChnY)+floor(sampBoxChnHeight),...
            round(sampBoxChnX):round(sampBoxChnX)+floor(sampBoxChnWidth));
        % Sorting Positives And Negatives
        positives{m} = samps{m}(maskDown);
        negatives{m} = samps{m}(~maskDown);
    end
    
    %% Segment Current Frame
    Hists = cell(10,2);
    numBin = 50;
    for m=1:10
        binEdges = linspace(min(min(samps{m})),max(max(samps{m})),numBin+1);
        Hists{m,1} = histcounts( positives{m} ,'Normalization','probability','BinEdges',binEdges);
        Hists{m,2} = histcounts( Is{m},'Normalization','probability','BinEdges',binEdges);
        % Finding the right bin
        reclass = reshape(samps{m},[],1);
        for j=1:size(reclass,1)
            
            bin = find((binEdges > reclass(j)),1)-1;
            if bin == 0
                bin = 1;
            end
            
            if Hists{m,1}(bin) >= Hists{m,2}(bin)
                confidence(m,j) = Hists{m,1}(bin);
            else 
                confidence(m,j) = 0;
            end
        end
        confidence(m,:) = confidence(m,:)./max(confidence(m,:));

    end
    totConf = sum(confidence,1);
    totConf = reshape(totConf,size(samps{1}));
    totConf = totConf./max(max(totConf));

    % Estimating the probability density
    f_1 = zeros(10,100);
    f_0 = zeros(10,100);
    
    xi = zeros(10,100);
    for m=1:10
        xi(m,:) = linspace(min(min(samps{m})),max(max(samps{m})),100);
        [f_1_tmp, xi_1_tmp] = ksdensity(positives{1,m},xi(m,:),'Kernel','epanechnikov');
        [f_0_tmp, xi_0_tmp] = ksdensity(reshape(samps{m},[],1)',xi(m,:),'Kernel','epanechnikov');
        f_1(m,:) = f_1_tmp;
        f_0(m,:) = f_0_tmp;
        
        %% Finding the confidence values ----------------------------------
        sampTmp = reshape(samps{m},[],1);
        for r=1:length(sampTmp)
            factors = 0.5:0.25:2;
            cnt = 1;
            if  sampTmp(r) < xi(m,1)
                index = 2;
            elseif sampTmp(r) > xi(m,end)
                index = length(xi(m));
            else
                index = find((abs(xi(m,:)-sampTmp(r))<0.25*(xi(m,2)-xi(m,1)))== 1);
            end
            while isempty(index)
                index = find((abs(xi(m,:)-sampTmp(r))<factors(cnt)*(xi(m,2)-xi(m,1)))== 1);
                cnt = cnt+1;
            end
            if index == 1
                index = 2;
            end
            if size(index,2)>1
                index = index(1);
            end
            
            if f_1(m,index) >= f_0(m,index)
                pdf(m,r) = f_1(m,index);
                probs(m,r) = trapz(xi(m,1:index),f_1(m,1:index));
            else
                pdf(m,r) = 0;
                probs(m,r) = 0;
            end
        end   
        probs(m,:) = probs(m,:)./max(probs(m,:));
        %% ----------------------------------------------------------------
    end

    resProb = sum(probs,1);
    resProb = reshape(resProb,size(samps{1}));
    resProb = resProb./max(max(resProb));
%     figure;imagesc(resProb);colorbar;title('Segmentation using PDF');
        
    %% Training a Robost Boosting and Adaboosting Classifier
    
    if frame == 1 && flag.trainClassifier == 1
        trainX_1 = [positives{1:10}];
        trainX_0 = [negatives{1:10}];
        trainY_1 = ones(size(trainX_1,1),1);
        trainY_0 = zeros(size(trainX_0,1),1);
        
        trainX = [trainX_1;trainX_0];
        trainY = [trainY_1;trainY_0];
        
        tic;
        adaBoost = fitensemble(trainX,trainY,'AdaBoostM1',size(trainX,1),'Tree');
        t_ada = toc;
%         tic
%         robuBoost = fitensemble(trainX,trainY,'RobustBoost',size(trainX_1,1),'Tree');
%         t_robu = toc;
        
        disp(['time to train adaBoost   : ' num2str(t_ada)]);
%         disp(['time to train robustBoost: ' num2str(t_robu)]);
    end
    
    %% Classification Test
    if flag.testClassifier == 1
        posMat = [positives{1:10}];
        negMat = [negatives{1:10}];
        sampMat = zeros(100,10);
        for m=1:10
            sampMat(:,m) = reshape(samps{m},[],1);
        end
        testSamps = -1*ones(size(sampMat,1),1);
        testConf = zeros(size(sampMat,1),2);
        testScores = zeros(size(sampMat,1),1);
        testerAdaBoostPos = -1*ones(size(posMat,1),1);
%         testerRobuBoostPos = 10*ones(size(posMat,1),1);
        testerAdaBoostNeg = -1*ones(size(negMat,1),1);
%         testerRobuBoostNeg = 10*ones(size(negMat,1),1);
        for m=1:size(sampMat,1)
            [testSamps(m),testConf(m,:)] = predict(adaBoost,sampMat(m,:));
            if testSamps(m) == 1
                testScores(m) = abs(testConf(m,1));
            end
        end
        
        segResult = reshape(testScores,[10,10]);
        
        figure;imagesc(segResult);colorbar;

%         tic
%         for m=1:size(posMat,1)
%             testerAdaBoostPos(m) = predict(adaBoost,posMat(m,:));
% %             testerRobuBoostPos(m) = predict(robuBoost,posMat(m,:));
%         end
%         t_1 = toc;
%         tic
%         for m=1:size(negMat,1)
%             testerAdaBoostNeg(m) = predict(adaBoost,negMat(m,:));
% %             testerRobuBoostNeg(m) = predict(robuBoost,negMat(m,:));
%         end
%         t_0 = toc;
%         
%         disp(['Classify frame' num2str(frame)]);
%         disp(['time to predict adaBoost   : ' num2str(t_1+t_0)]);
%         disp(['time to predict robustBoost: ' num2str(t_robu)]);
        
        errorRateAda(frame) = (sum((testerAdaBoostPos == 0))+sum((testerAdaBoostNeg == 1)))/...
            (length(testerAdaBoostPos)+length(testerAdaBoostNeg));
%         errorRateRobu(frame) = (sum((testerRobuBoostPos == 0))+sum((testerRobuBoostNeg == 1)))/...
%             (length(testerRobuBoostPos)+length(testerRobuBoostNeg));
        
    end
    
    %% Plotting Image and Feature Channels
    if flag.plotGt == 1
        figure('Name','Ground Truth');
        imshow(currFrame);
        patch('Faces',[1,2,3,4],'Vertices',v,'EdgeColor','blue',...
            'FaceColor','none','LineWidth',2);
        rectangle('Position',[minX,minY,maxX-minX,maxY-minY],'LineWidth',2,'EdgeColor','r');
        rectangle('Position',[sampBoxImgX,sampBoxImgY,sampBoxImgWidth,sampBoxImgHeight],'LineWidth',2,'EdgeColor','c');
    end
    if flag.plotMask == 1
        figure('Name','GroundTruth Original')
        imshow(mask);
        title('GroundTruth Mask in Original Size');
        figure('Name','GroundTruth Operations')
        subplot(1,2,1);
        imshow(maskBox);
        title('Cutting Out');
        subplot(1,2,2);
        imshow(maskDown);
        title('Cutting Out and Sampling Down');
    end
    if flag.plotChns == 1
        figure('Name','Multiple Channels');
        subplot(5,2,1);
        imshow(chns.data{1,1}(:,:,1));
        title('Channel 1: L');
        subplot(5,2,2);
        imshow(chns.data{1,1}(:,:,2));
        title('Channel 2: U');
        subplot(5,2,3);
        imshow(chns.data{1,1}(:,:,3));
        title('Channel 3: V');
        subplot(5,2,4);
        imshow(chns.data{1,2}(:,:));
        title('Channel 4: Mag');
        subplot(5,2,5);
        imshow(chns.data{1,3}(:,:,1));
        title('Channel 5: HOG');
        subplot(5,2,6);
        imshow(chns.data{1,3}(:,:,2));
        title('Channel 6: HOG');
        subplot(5,2,7);
        imshow(chns.data{1,3}(:,:,3));
        title('Channel 7: HOG');
        subplot(5,2,8);
        imshow(chns.data{1,3}(:,:,4));
        title('Channel 8: HOG');
        subplot(5,2,9);
        imshow(chns.data{1,3}(:,:,5));
        title('Channel 9: HOG');
        subplot(5,2,10);
        imshow(chns.data{1,3}(:,:,6));
        title('Channel 10: HOG');
    end
    if flag.plotLocalSum == 1
        figure('Name','Feature Channels');
        for m = 1:10
            subplot(5,2,m);
            imagesc(Is{m});
            colorbar;
            title(['Sums ' num2str(m)]);
        end
    end
    if flag.plotFtr == 1
        figure('Name','Sample Sums');
        for m = 1:10
            subplot(5,2,m);
            imagesc(samps{m});
            colorbar;
            title(['Features in ROI Channel: ' num2str(m)]);
        end
    end
    if flag.plotHist == 1
        figure('Name','Histograms');
        for m=1:10
            binEdges = linspace(min(min(samps{m})),max(max(samps{m})),numBin+1);
            subplot(5,2,m);
            h1 = histogram( positives{m} ,'Normalization','pdf','BinEdges',binEdges);
            hold on
            h2 = histogram(samps{m},'Normalization','pdf','BinEdges',binEdges);
            plot(xi(m,:),f_1(m,:),'LineWidth',2);
            plot(xi(m,:),f_0(m,:),'LineWidth',2);
            grid;
            title(['Histograms in Channel ' num2str(m)]);
            xlabel('ftr value');
            ylabel('pdf');
            %legend('positive ftrs','negative ftrs');
        end
    end
    if flag.plotResults == 1
        figure;imagesc(totConf);colorbar;title('Segmentation using histograms');
        
%         thresh = imresize(thresh,[sampBoxImgHeight,sampBoxImgWidth]);
        totConf = imresize(totConf,[sampBoxImgHeight,sampBoxImgWidth]);
        thresh = im2bw(totConf, 0.55);
        figure;
        imagesc(totConf);colorbar;title('Resulting Heatmap');
        figure;
        imshow(thresh);title('Confidence Threshold 0.55');
        
        
    end        
         
end
    