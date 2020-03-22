using UnityEngine;

public class PlayerController : MonoBehaviour
{
    [SerializeField] private Vector2 attackSpeed, moveSpeed = new Vector2(1,1);
    [SerializeField] private GameObject spaceRotation, attackVFX;
    [SerializeField] private float fireThreshold;
    private Vector2 playerSpeed;
    private Quaternion baseRotation;
    // Start is called before the first frame update
    void Start()
    {
        baseRotation = transform.rotation;
        
    }
    

    // Update is called once per frame
    void Update()
    {
        var attacking = Attacking();
        Move(attacking);
        Attack(attacking);
    }

    private void Move(bool attacking)
    {
        playerSpeed = attacking ? attackSpeed : moveSpeed;
        float forward, turning;

        turning = (FaceTracker.input.x);
        forward = (FaceTracker.input.y);
        //if(Mathf.Abs(FaceTracker.facePosition.x - 0.5f) <= 0.0f)
        //    turning = (FaceTracker.facePosition.x - 0.5f) * 2;
        //else    
        //    turning = Input.GetAxis("Horizontal");

        //if (Mathf.Abs(FaceTracker.facePosition.y - 0.5f) <= 0.0f)
        //    forward = (FaceTracker.facePosition.y - 0.5f) * 2;
        //else
        //    forward = Input.GetAxis("Vertical");

        forward *= playerSpeed.x * Time.deltaTime;
        turning *= playerSpeed.y * Time.deltaTime;

        spaceRotation.transform.Rotate(-forward, -turning, 0, Space.World);

        //Creates the wobble effect on the UFO
        transform.rotation = Quaternion.Euler(baseRotation.eulerAngles + new Vector3(Mathf.Cos(Time.fixedTime * 5) * 3 + forward, Mathf.Sin(Time.fixedTime * 13) * 7, Mathf.Sin(Time.fixedTime * 19) * 4 + turning));

    }

    private void Attack(bool attacking)
    {
        attackVFX.SetActive(attacking);
        if (attacking)
        {
            //Fire button activated
            var ray = new Ray(transform.position, Vector3.down);
            RaycastHit[] hitUnits = Physics.RaycastAll(ray, 10f);

            foreach (var hit in hitUnits)
            {
                Debug.Log(hit.collider.gameObject.name);
            }

            Debug.DrawRay(transform.position, Vector3.down, Color.red, 1f);
        }
    }

    public bool Attacking()
    {
        return Mathf.Abs(Input.GetAxis("Fire1")) > fireThreshold;
    }
}

//public partial class Form1 : Form
//{
//    static readonly CascadeClassifier casClass  new CascadeClassifier("haarcascade_frontalface_alt_tree.xml");
//}
