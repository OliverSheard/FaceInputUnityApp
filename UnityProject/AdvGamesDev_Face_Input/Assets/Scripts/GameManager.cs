using System;
using System.Collections;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using UnityEngine;

public class GameManager : MonoBehaviour
{
    [DllImport("FaceInput", EntryPoint = "?getTickFrequency@cv@@YANXZ")]
    private static extern int main();

    public static EventHandler<GameUpdate> scoreUpdateEvent;
    private int score;

    private List<GameObject> activeEnemies, activeBases;

    // Start is called before the first frame update
    void Start()
    {
        activeEnemies = new List<GameObject>();
        activeBases = new List<GameObject>();
        scoreUpdateEvent += ScoreUp;
        main();
    }

    private void ScoreUp(object sender, GameUpdate e)
    {
        score += e.Points;
    }

    // Update is called once per frame
    void Update()
    {
        
    }

    public class GameUpdate : EventArgs
    {
        public int Points { get; private set; }

        public GameUpdate(int score)
        {
            Points = score;
        }
    }
}
