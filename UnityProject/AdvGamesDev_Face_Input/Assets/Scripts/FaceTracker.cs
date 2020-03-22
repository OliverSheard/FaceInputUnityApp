using System.Collections;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using UnityEngine;
using System.Linq;

public class FaceTracker : MonoBehaviour
{
    public static Vector2 facePosition, nosePosition, input;
    public static Vector2[] eyePositions;
    [Range(0,10)][SerializeField] private int _maxEyes;
    [SerializeField] private bool showCapture = false;
    private OpenCVCircle[] _eyes;
    private OpenCVCircle _faces, _nose;
    private bool active = false;
    private int camWidth, camHeight;
    // Start is called before the first frame update
    void Start()
    {
        ZeroInput();
        int camIndex;

        camIndex = camWidth = camHeight = 0;

        int result = OpenCVFace.Initialise(ref camIndex, ref camWidth, ref camHeight);

        Debug.Log(camWidth + ":" + camHeight);

        _eyes = new OpenCVCircle[_maxEyes];

        eyePositions = new Vector2[_maxEyes];

        if (result == 0)
            active = true;
    }

    private static void ZeroInput()
    {
        facePosition = nosePosition = input = new Vector2(0, 0);
        eyePositions = new Vector2[] { new Vector2(0, 0), new Vector2(0, 0) };
    }

    private void OnApplicationQuit()
    {
        if (active)
            OpenCVFace.Release();
    }

    void Update()
    {
        if(active)
        {
            DetectFrontFaceAndEyes();
        }
    }

    private void DetectFrontFaceAndEyes()
    {
        int result = 1;
        unsafe
        {
            fixed (OpenCVCircle* eyesDetected = _eyes)
            {
                result = OpenCVFace.DetectFace(ref _faces, eyesDetected, ref _nose, _maxEyes, showCapture);
            }
        }

        //Can switch case the output, eg -1 means no face detected etc. then can zero the player input or potentially pause if face is not detected
        if (result == 0)
        {
            facePosition = new Vector2(_faces.X, _faces.Y);            
            eyePositions = _eyes.Select(s => new Vector2((float)s.X / camWidth, (float)s.Y / camHeight)).ToArray();
            nosePosition = new Vector2(_nose.X, _nose.Y);

            input = facePosition - nosePosition;

            Debug.Log("Delta X = " + input.x + ": Delta Y = " + input.y);

        }
        else
        {
            ZeroInput();
        }
    }
}
internal static class OpenCVFace
{
    [DllImport("FaceInput")]
    internal static extern int Initialise(ref int camIndex, ref int camWidth, ref int camHeight);

    [DllImport("FaceInput")]
    internal unsafe static extern int DetectFace(ref OpenCVCircle facePos, OpenCVCircle* eyes,ref OpenCVCircle nose, int maxEyes, bool showCap);

    [DllImport("FaceInput")]
    internal static extern void Release();
}

[StructLayout(LayoutKind.Sequential, Size = 96)]
public struct OpenCVCircle
{
    public int X, Y, Radius;
}
