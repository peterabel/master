%%% Generating ConfMap
clc;clear;close all;

addpath(genpath('G:\abel\confMap\outlier detection'));

%loading
filename = 'girl';
numMaxVal = 80;

trackLik = load([filename '_trackLik.txt']);
trackLik(:,1:2) = trackLik(:,1:2)+1;
selectorsRaw = load([filename '_selectors.txt']);
selectors = selectorsRaw+1;
ftrPosRaw = load([filename '_lll.txt']);
ftrPos = ftrPosRaw(:,2:5)+1;

posLik = trackLik(:,5:2:end);
negLik = trackLik(:,6:2:end);

minX = min(trackLik(:,1));
minY = min(trackLik(:,2));
trackLik(:,1) = trackLik(:,1)-minX;
trackLik(:,2) = trackLik(:,2)-minY;
maxX = max(trackLik(:,1)+trackLik(:,3));
maxY = max(trackLik(:,2)+trackLik(:,4));

trackResults = sum(trackLik(:,5:2:end),2);
[maxSample maxSampInd] = sort(trackResults,'descend');%+sum(trackLik(:,6:2:end),2)

trackResults_norm = abs(trackResults)/ max(abs(trackResults));

%total center point
centerPointX = trackLik(maxSampInd(1),2)+0.5*trackLik(maxSampInd(1),4);%rawData(1,6)+0.5*rawData(1,8);
centerPointY = trackLik(maxSampInd(1),3)+0.5*trackLik(maxSampInd(1),5);%rawData(1,7)+0.5*rawData(1,9);
%all center points
cpsX = ftrPosRaw(:,2)+0.5*ftrPosRaw(:,4);
cpsY = ftrPosRaw(:,3)+0.5*ftrPosRaw(:,5);
%euclidean differences to the middle
dist_x = sqrt((cpsX-centerPointX).^2);
dist_y = sqrt((cpsY-centerPointY).^2);
dist = sqrt((cpsX-centerPointX).^2+(cpsY-centerPointY).^2);
dist_norm = dist/max(dist);


result_pos = zeros(maxY+2,maxX+2);
result_neg = zeros(maxY+2,maxX+2);
for s=1:20%:size(trackLik,1)
    samp = maxSampInd(s);
    for f=1:numMaxVal
        xMin = trackLik(samp,1)+ ftrPos(selectors(f),1);
        xMax = trackLik(samp,1)+ ftrPos(selectors(f),1)+ftrPos(selectors(f),3);
        yMin = trackLik(samp,2)+ ftrPos(selectors(f),3);
        yMax = trackLik(samp,2)+ ftrPos(selectors(f),2)+ftrPos(selectors(f),4);
        if(yMax-1 < maxY)
            for y=yMin:yMax-1
                if(xMax-1 < maxX)
                    for x=xMin:xMax-1
                        %if(posLik(samp,f)>=negLik(samp,f))
                            p = posLik(samp,f)-negLik(samp,f);
                            sigVal = 0.5 + 0.5*p/(abs(p));
                            result_pos(y,x)=result_pos(y,x)+trackResults_norm(s)*sigVal*dist_norm(f);%+negLik(samp,f)/size(trackLik,1);  
                        %else
                         %   result_pos(y,x)=result_pos(y,x)-negLik(samp,f);
                        %end
                    end;
                end
            end;
        end
    end;
end;


im = abs(result_pos)/max(max(abs(result_pos)));

se = strel('disk',20);
closeBW = imclose(im,se);


thresh = 0.2;
threshImg = im2bw(im,thresh);

figure();imshow(im);
figure();imshow(closeBW);
figure();imshow(threshImg);
figure();imagesc(closeBW);colorbar();
% 
% 
% % %total center point
% % centerPointX = lll(1,6)+0.5*lll(1,8);%lll(indLll,2)+0.5*lll(indLll,4);%
% % centerPointY = lll(1,7)+0.5*lll(1,9);%lll(indLll,3)+0.5*lll(indLll,5);
% % %all center points
% % cpsX = lll(:,2)+0.5*lll(:,4);
% % cpsY = lll(:,3)+0.5*lll(:,5);
% % %euclidean differences to the middle
% % dist_x = sqrt((cpsX-centerPointX).^2);
% % dist_y = sqrt((cpsY-centerPointY).^2);
% % dist = sqrt((cpsX-centerPointX).^2+(cpsY-centerPointY).^2);
% % 
% % dist_norm = dist./max(dist);
% % 
% % numMaxVal = 120;
% % shiftDown = 20;
% % 
% % [maxVal maxInd] = sort(lll(:,1),'descend');
% % maxVal = lll(maxInd((1+shiftDown):(numMaxVal+shiftDown)),:);
% % dist_norm = dist_norm(maxInd((1+shiftDown):(numMaxVal+shiftDown)),:);
% % 
% % result = zeros(max(maxVal(:,3)+maxVal(:,5)),max(maxVal(:,2)+maxVal(:,4)));
% % for i=1:numMaxVal
% %     for y=maxVal(i,3)+1:(maxVal(i,3)+maxVal(i,5))
% %         for x=maxVal(i,2)+1:(maxVal(i,2)+maxVal(i,4))
% %             result(y,x)=result(y,x)+maxVal(i,1)/(dist_norm(i,1)+1e-3);
% %         end;
% %     end;
% % end;