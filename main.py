from tensorflow.keras.applications.mobilenet_v2 import preprocess_input
from tensorflow.keras.preprocessing.image import img_to_array
from imutils.video import VideoStream
import numpy as np
import tensorflow as tf
import cv2
import imutils
import gtts
from hwutils import DoorController
from playsound import playsound
import _thread

# load our serialized face detector model from disk
prototxtPath = r"face_detector/deploy.prototxt"
weightsPath = r"face_detector/res10_300x300_ssd_iter_140000.caffemodel"
_faceModel = cv2.dnn.readNet(prototxtPath, weightsPath)
_maskModel = tf.keras.models.load_model('mask_detector.model', custom_objects=None, compile=True)


class MaskDetector():

    def __init__(self):
        pass

    def __call__(self, frame):
        # grab the dimensions of the frame and then construct a blob
        # from it
        (h, w) = frame.shape[:2]
        blob = cv2.dnn.blobFromImage(frame, 1.0, (224, 224),
                                     (104.0, 177.0, 123.0))

        # pass the blob through the network and obtain the face detections
        _faceModel.setInput(blob)
        detections = _faceModel.forward()

        # initialize our list of faces, their corresponding locations,
        # and the list of predictions from our face mask network
        faces = []
        locs = []
        preds = []

        # loop over the detections
        for i in range(0, detections.shape[2]):
            # extract the confidence (i.e., probability) associated with
            # the detection
            confidence = detections[0, 0, i, 2]

            # filter out weak detections by ensuring the confidence is
            # greater than the minimum confidence
            if confidence > 0.5:
                # compute the (x, y)-coordinates of the bounding box for
                # the object
                box = detections[0, 0, i, 3:7] * np.array([w, h, w, h])
                (startX, startY, endX, endY) = box.astype("int")

                # ensure the bounding boxes fall within the dimensions of
                # the frame
                (startX, startY) = (max(0, startX), max(0, startY))
                (endX, endY) = (min(w - 1, endX), min(h - 1, endY))

                # extract the face ROI, convert it from BGR to RGB channel
                # ordering, resize it to 224x224, and preprocess it
                face = frame[startY:endY, startX:endX]
                face = cv2.cvtColor(face, cv2.COLOR_BGR2RGB)
                face = cv2.resize(face, (224, 224))
                face = img_to_array(face)
                face = preprocess_input(face)

                # add the face and bounding boxes to their respective
                # lists
                faces.append(face)
                locs.append((startX, startY, endX, endY))

        # only make a predictions if at least one face was detected
        if len(faces) > 0:
            # for faster inference we'll make batch predictions on *all*
            # faces at the same time rather than one-by-one predictions
            # in the above `for` loop

            #print('OCC----------- = ',occupancy)
            # check if occupancy is reached
            faces = np.array(faces, dtype="float32")
            preds = _maskModel.predict(faces, batch_size=32)

            #tts = gtts.gTTS("Nut Overflow")
            #tts.save("nut.mp3")
            #playsound("nut.mp3")

        # return a 2-tuple of the face locations and their corresponding
        # locations
        return (locs, preds)


print("[INFO] starting video stream...")
detector = MaskDetector()
controller = DoorController()
vs = VideoStream(src=0).start()
once = False
maskWait = False
# loop over the frames from the video stream
while True:
    # grab the frame from the threaded video stream and resize it
    # to have a maximum width of 400 pixels
    frame = vs.read()
    frame = imutils.resize(frame, width=400)

    # detect faces in the frame and determine if they are wearing a
    # face mask or not
    (locs, preds) = detector(frame)

    # loop over the detected face locations and their corresponding
    # locations

    if len(preds) > 0:
        mask, noMask = preds[0]

        occupancy = controller.check_occupancy()
        print('BOOOO0=F',occupancy)
        if occupancy < 3: 

            if noMask > mask and not maskWait:
                controller.dispenseMask()
                controller.setLed(False)
                maskWait = True
                # send signal to the dispenser

            elif mask > noMask and maskWait:
                controller.setLed(True)
                maskWait = False

    if len(preds) == 0 and once:
        print('nothing seen, resetting')

    for (box, pred) in zip(locs, preds):
        # unpack the bounding box and predictions
        (startX, startY, endX, endY) = box
        (mask, withoutMask) = pred

        # determine the class label and color we'll use to draw
        # the bounding box and text
        label = "Mask" if mask > withoutMask else "No Mask"
        color = (0, 255, 0) if label == "Mask" else (0, 0, 255)

        # include the probability in the label
        label = "{}: {:.2f}%".format(label, max(mask, withoutMask) * 100)

        # display the label and bounding box rectangle on the output
        # frame
        cv2.putText(frame, label, (startX, startY - 10),
                    cv2.FONT_HERSHEY_SIMPLEX, 0.45, color, 2)
        cv2.rectangle(frame, (startX, startY), (endX, endY), color, 2)

    # show the output frame
    cv2.imshow("Frame", frame)
    key = cv2.waitKey(1) & 0xFF

    # if the `q` key was pressed, break from the loop
    if key == ord("q"):
        break

# do a bit of cleanup
cv2.destroyAllWindows()
vs.stop()
