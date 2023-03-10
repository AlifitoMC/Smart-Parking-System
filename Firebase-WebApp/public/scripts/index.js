const loginElement = document.querySelector('#login-form');
const contentElement = document.querySelector("#content-sign-in");
const userDetailsElement = document.querySelector('#user-details');
const authBarElement = document.querySelector("#authentication-bar");
var button = document.getElementById("buttonStyle");
var drop = document.getElementById("Space");
var overlay = document.querySelector('#overlay');
var yesButton = document.querySelector('#yesButton');
var noButton = document.querySelector('#noButton');


// MANAGE LOGIN/LOGOUT UI
const setupUI = (user) => {
  if (user) {
    //toggle UI elements
    loginElement.style.display = 'none';
    contentElement.style.display = 'block';
    authBarElement.style.display ='block';
    userDetailsElement.style.display ='block';
    userDetailsElement.innerHTML = user.email;
    document.getElementById("reservation_form").addEventListener('submit', makeReservation);


    // get user UID to get data from database
    var uid = user.uid;
    console.log(uid);

    // Database paths (with user UID)
    var dbPathDist = 'UsersData/' + uid.toString() + '/distance';
    var reservePath = 'UsersData/' + uid.toString() + '/reservation_form';
    
    // Database references
    var dbRefDist = firebase.database().ref().child(dbPathDist);
    var dbRefReservationForm = firebase.database().ref().child(reservePath)

    // function to make a reservation
    function makeReservation(e){
      e.preventDefault();

      var Space = getElementVal('Space');
      var Reserve_Date = getElementVal("Reserve_Date");

      console.log(Space, Reserve_Date);

      saveReservation(Space, Reserve_Date);

      // alert message to show successful submission
      const alertPlaceholder = document.getElementById('liveAlertPlaceholder')

      const alert = (message, type) => {
        const wrapper = document.createElement('div')
        wrapper.innerHTML = [
          `<div class="alert alert-${type} alert-dismissible" role="alert">`,
          `   <div>${message}</div>`,
          '   <button type="button" class="btn-close" data-bs-dismiss="alert" aria-label="Close"></button>',
          '</div>'
        ].join('')
      
        alertPlaceholder.append(wrapper)
      }

    alert('Reservation Successful!', 'success')
    
    //reset the form
    document.getElementById("reservation_form").reset();
    }

    // function to get submitted data e.g. space and date
    const getElementVal = (id) => {
      return document.getElementById(id).value;
    };

    // function to save the reservation to the database
    const saveReservation = (Space, Reserve_Date) => {
      var newReservation = dbRefReservationForm.push();

      newReservation.set({
        Space : Space,
        Reserve_Date : Reserve_Date
      });
    };

    dbRefDist.on("value",function(snapshot) {
      var dist = snapshot.val();
    
    if (dist < 9) {
      button.classList.remove("btn-success");
      button.classList.remove("btn-primary");
      button.classList.add("btn-danger")
      drop.options[1].style.display="none";
      document.getElementById("overlay").style.display = "none";
    } else {
      button.classList.remove("btn-danger");
      button.classList.remove("btn-primary");
      button.classList.add("btn-success");
      drop.options[1].style.display="block";
    }
    });

    dbRefReservationForm.on("value", function(snapshot) {
      var value = snapshot.val();
      
      if (value) {
        button.classList.remove("btn-success");
        button.classList.add("btn-primary");
        drop.options[1].style.display="none";
        document.getElementById("overlay").style.display = "block";
      } else{
        button.classList.remove("btn-primary");
        button.classList.add("btn-success");
        drop.options[1].style.display="block";
      }
    });

    yesButton.addEventListener('click', () => overlay.style.display = 'none');
    yesButton.addEventListener('click', () => dbRefReservationForm.remove());
    noButton.addEventListener('click', () => overlay.style.display = 'none');
    
    // if user is logged out
  } else{
    // toggle UI elements
    loginElement.style.display = 'block';
    authBarElement.style.display ='none';
    userDetailsElement.style.display ='none';
    contentElement.style.display = 'none';
  }
}