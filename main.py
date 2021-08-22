import paho.mqtt.client as mqtt
from queue import Queue

from influxdb_client import InfluxDBClient, Point
from influxdb_client.client.write_api import SYNCHRONOUS

# using a queue so that we read all the incoming messages
q = Queue()


def on_connect(client, userdata, flags, rc):
    """
    overriding the on_connect function
    :param client: that we are connecting to
    :param userdata:
    :param flags:
    :param rc: connection feedback
    :return:
    """
    if rc == 0:
        print("connected ok")
    else:
        print("Bad connection. Returned code: ", rc)


def on_message(client, userdata, msg):
    """
    on_message function override
    :param client:
    :param userdata:
    :param msg: received message
    :return:
    """
    q.put(msg)
    print("message received ", str(msg.payload.decode("utf-8")))
    print("message topic =", msg.topic)


def on_log(client, userdata, level, buf):
    """
    on_log function override - general diagnostic function
    :param client:
    :param userdata:
    :param level:
    :param buf:
    :return:
    """
    print("log: ", buf)


def on_disconnect(client, userdata, rc=0):
    """
    on_disconnect function override
    :param client:
    :param userdata:
    :param rc:
    :return:
    """
    print("Disconnected result code "+str(rc))
    client.loop_stop()


def main():

    # my special details and credentials for the influx database
    token = "NxnKD1XB8QdD_uyYf-XaMJsr8cHoLf-Oe4Qae6LcVUmr7RJntKFu1UniPrNS9rvbIz2TT24Zn_34K1-fYln8Jg=="
    org = "University"
    bucket = "iot"

    # creating client and write api
    db_client = InfluxDBClient(url="http://localhost:8086", token=token)
    write_api = db_client.write_api(write_options=SYNCHRONOUS)

    # connecting functions to the instance
    client = mqtt.Client("x1")
    client.on_message = on_message
    client.on_disconnect = on_disconnect
    client.on_log = on_log

    # connecting to mqtt broker
    print("connecting to broker")
    client.connect("localhost", port=1883)
    client.subscribe("parking/#")
    client.loop_start()

    try:
        while True:

            # processing incoming messages until there is no more in the queue
            while not q.empty():
                message = q.get()
                if message is None:
                    continue

                # decoding the message and splitting the string by topic fields
                msg = message.payload.decode("utf-8")
                msg = float(msg)
                topic = message.topic
                topic_fields = topic.split("/")

                # creating a new Point instance to send to the database
                point = Point(topic_fields[3]).tag("Garage", topic_fields[1]).tag("Slot", topic_fields[2])\
                    .field("value", msg)

                # sending the new point to the influx database
                write_api.write(bucket, org, point)

    # stopping the loop with a keyboard interrupt
    except KeyboardInterrupt:
        client.loop_stop()
        pass


if __name__ == '__main__':
    main()
