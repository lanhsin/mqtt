#include <mosquitto.h>
#include "mqtt_protocol.h"
#include "network.h"    // get_macAddr
#include <iostream>
#include <errno.h>
#include <string.h>

/* Callback called when the client receives a CONNACK message from the broker. */
void on_connect(struct mosquitto *mosq, void *obj, int reason_code)
{
    /* Print out the connection result. mosquitto_connack_string() produces an
    * appropriate string for MQTT v3.x clients, the equivalent for MQTT v5.0
    * clients is mosquitto_reason_string().
    */
	printf("on_connect: %s\n", mosquitto_connack_string(reason_code));
	if(reason_code != 0){
		/* If the connection fails for any reason, we don't want to keep on
			* retrying in this example, so disconnect. Without this, the client
			* will attempt to reconnect. */
		mosquitto_disconnect(mosq);
	}

	/* Making subscriptions in the on_connect() callback means that if the
	* connection drops and is automatically resumed by the client, then the
	* subscriptions will be recreated when the client reconnects. */
	const char* sub0 = "dnp3/inv";
	const char* sub1 = "dnp3/env";
	const size_t topic_count = 2;
	char ** topics = (char**)malloc(topic_count * sizeof(char *));
	topics[0] = strdup(sub0);
	topics[1] = strdup(sub1);
	int rc = mosquitto_subscribe_multiple(mosq, nullptr, 2, topics, 2, MQTT_SUB_OPT_NO_LOCAL, nullptr);

	for(int i = 0; i < topic_count; i++)
		free(topics[i]);

	if(rc != MOSQ_ERR_SUCCESS){
		fprintf(stderr, "Error subscribing: %s\n", mosquitto_strerror(rc));
		/* We might as well disconnect if we were unable to subscribe */
		mosquitto_disconnect(mosq);
	}
}


/* Callback called when the broker sends a SUBACK in response to a SUBSCRIBE. */
void on_subscribe(struct mosquitto *mosq, void *obj, int mid, int qos_count, const int *granted_qos)
{
    bool have_subscription = false;

    /* In this example we only subscribe to a single topic at once, but a
     * SUBSCRIBE can contain many topics at once, so this is one way to check
     * them all. */
	for(int i = 0; i < qos_count; i++){
		printf("on_subscribe: %d:granted qos = %d\n", i, granted_qos[i]);
		if(granted_qos[i] <= 2){
			have_subscription = true;
		}
	}
	if(have_subscription == false){
		/* The broker rejected all of our subscriptions, we know we only sent
		 * the one SUBSCRIBE, so there is no point remaining connected. */
		fprintf(stderr, "Error: All subscriptions rejected.\n");
		mosquitto_disconnect(mosq);
	}
}

/* Callback called when the client receives a message. */
void on_message(struct mosquitto *mosq, void *obj, const struct mosquitto_message *msg)
{
	/* This blindly prints the payload, but the payload can be anything so take care. */
	printf("%s %d %s\n", msg->topic, msg->qos, (char *)msg->payload);
}

int main(void)
{
	struct mosquitto *mosq;
	int rc;

	/* Required before calling other mosquitto functions */
	mosquitto_lib_init();

	/* Create a new client instance.
	 * id = mac address
	 * clean session = true -> the broker should remove old sessions when we connect
	 * obj = std::shared_ptr<opendnp3::IOutstation>* outstation -> we aren't passing any of our private data for callbacks
	 */
    char client_id[20] = {0};
	get_macAddr(client_id);
	mosq = mosquitto_new(client_id, true, nullptr);
	if(mosq == nullptr){
        switch(errno){
            case ENOMEM:
                std::cerr <<"Error: Out of memory.\n";
                break;
            case EINVAL:
                std::cerr <<"Error: Invalid id.\n";
                break;
		}
        mosquitto_lib_cleanup();
		return 1;
	}

	/* Configure callbacks. This should be done before connecting ideally. */
	mosquitto_connect_callback_set(mosq, on_connect);
	mosquitto_subscribe_callback_set(mosq, on_subscribe);
	mosquitto_message_callback_set(mosq, on_message);

    mosquitto_int_option(mosq, MOSQ_OPT_PROTOCOL_VERSION, MQTT_PROTOCOL_V5);

	/* Connect to test.mosquitto.org on port 1883, with a keepalive of 60 seconds.
	 * This call makes the socket connection only, it does not complete the MQTT
	 * CONNECT/CONNACK flow, you should use mosquitto_loop_start() or
	 * mosquitto_loop_forever() for processing net traffic. */
	rc = mosquitto_connect(mosq, "localhost", 1883, 60);
	if(rc != MOSQ_ERR_SUCCESS){
		mosquitto_destroy(mosq);
		std::cerr << "Error: " << mosquitto_strerror(rc) <<"\n";
		return 1;
	}

	/* Run the network loop in a blocking call. The only thing we do in this
	 * example is to print incoming messages, so a blocking call here is fine.
	 *
	 * This call will continue forever, carrying automatic reconnections if
	 * necessary, until the user calls mosquitto_disconnect().
	 */
	mosquitto_loop_forever(mosq, -1, 1);

	mosquitto_lib_cleanup();

	return 0;
}