using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class PlayerController : MonoBehaviour
{
    [SerializeField] private Vector2 playerSpeed = new Vector2(1,1);
    [SerializeField] private GameObject spaceRotation;
    private Quaternion baseRotation;
    // Start is called before the first frame update
    void Start()
    {
        baseRotation = transform.rotation;
    }
    

    // Update is called once per frame
    void Update()
    {
        Move();
    }

    private void Move()
    {
        var forward = Input.GetAxis("Vertical") * playerSpeed.x * Time.deltaTime;
        var turning = Input.GetAxis("Horizontal") * playerSpeed.y * Time.deltaTime;

        spaceRotation.transform.Rotate(-forward, -turning, 0, Space.World);

        transform.rotation = Quaternion.Euler(baseRotation.eulerAngles + new Vector3(Mathf.Cos(Time.fixedTime * 5) * 3, Mathf.Sin(Time.fixedTime * 13) * 7, Mathf.Sin(Time.fixedTime * 19) * 4));

    }
}
