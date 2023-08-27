FROM osrf/ros:galactic-desktop

SHELL ["/bin/bash", "-c"]

WORKDIR /app

# build custom ROS 2 nodes
COPY docker ros2_ws/
RUN cd ros2_ws && \
    source /opt/ros/galactic/setup.bash && \
    colcon build

COPY PizzaRobot/ros_entrypoint.sh /

ENTRYPOINT ["/ros_entrypoint.sh"]   
CMD ["bash"]